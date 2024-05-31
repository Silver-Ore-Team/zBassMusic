#include "Channel.h"
#include "NH/Bass/Engine.h"
#include "Options.h"

namespace NH::Bass
{
    struct OnSyncPosition { HSTREAM Channel; const std::function<void()>& Function; };
    struct OnSyncWhenAudioEndsData { HSTREAM Channel; const std::function<void()>& Function; };
    struct OnSyncBeforeAudioEndsData { HSTREAM Channel; const std::function<void(double)>& Function; };

    Channel::Result<void> Channel::PlayInstant(const AudioFile& audioFile)
    {
        if (m_Stream > 0)
        {
            BASS_ChannelFree(m_Stream);
        }

        m_Stream = BASS_StreamCreateFile(true, audioFile.Buffer.data(), 0, audioFile.Buffer.size(), 0);
        if (!m_Stream)
        {
            int code = BASS_ErrorGetCode();
            log->Error("Could not create stream: {0}\n  error: {1}\n  at {2}:{3}", audioFile.Filename, Engine::ErrorCodeToString(code), __FILE__, __LINE__);
            return std::unexpected(Error{ IChannel::ErrorType::INVALID_BUFFER, code, Engine::ErrorCodeToString(code) });
        }

        BASS_ChannelStart(m_Stream);
        return {};
    }

    void Channel::StopInstant()
    {
        BASS_ChannelStop(m_Stream);
    }

    void Channel::SetVolume(float volume)
    {
        if (volume < 0.0f) { log->Warning("Clamping invalid volume {0} to 0.0f", volume); volume = 0.0f; }
        if (volume > 1.0f) { log->Warning("Clamping invalid volume {0} to 1.0f", volume); volume = 1.0f; }
        BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_VOL, volume);
    }

    void Channel::SlideVolume(float targetVolume, uint32_t time)
    {
        BASS_ChannelSlideAttribute(m_Stream, BASS_ATTRIB_VOL, targetVolume, time);
    }

    void Channel::SlideVolume(float targetVolume, uint32_t time, const std::function<void()>& onFinish)
    {
        SlideVolume(targetVolume, time);
        BASS_ChannelSetSync(m_Stream, BASS_SYNC_SLIDE, 0, OnSlideVolumeSyncCallFunction, (void*)&onFinish);
    }

    void Channel::SetDX8ReverbEffect(float reverbMix, float reverbTime, float inputGain, float highFreqRTRatio)
    {
        HFX effect = BASS_ChannelSetFX(m_Stream, BASS_FX_DX8_REVERB, 0);
        if (reverbMix < -96) { log->Warning("Clamping DX8Reverb reverbMix({0}) to -96", reverbMix); reverbMix = -96; }
        if (reverbMix > 0) { log->Warning("Clamping DX8Reverb reverbMix({0}) to 0", reverbMix); reverbMix = 0; }
        if (reverbTime < 0.001f) { log->Warning("Clamping DX8Reverb reverbTime({0}) to 0.001f", reverbTime); reverbTime = 0.001f; }
        if (reverbTime > 3000.0f) { log->Warning("Clamping DX8Reverb reverbTime({0}) to 3000.0f", reverbTime); reverbTime = 3000.0f; }
        BASS_DX8_REVERB reverb{
            inputGain, reverbMix, reverbTime, highFreqRTRatio
        };
        if (!BASS_FXSetParameters(effect, &reverb))
        {
            int code = BASS_ErrorGetCode();
            log->Error("Could not set DX8Reverb effect: {0}\n  error: {1}\n  at {2}:{3}", m_Stream, Engine::ErrorCodeToString(code), __FILE__, __LINE__);
        }
    }

    void Channel::OnPosition(double position, const std::function<void()>& callback)
    {
        size_t positionBytes = BASS_ChannelSeconds2Bytes(m_Stream, position);
        BASS_ChannelSetSync(m_Stream, BASS_SYNC_POS, positionBytes, OnPositionSyncCallFunction, (void*)new OnSyncPosition{ m_Stream, callback });
    }

    void Channel::OnAudioEnds(const std::function<void()>& onFinish)
    {
        BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, 0, OnAudioEndSyncCallFunction, (void*)new OnSyncWhenAudioEndsData{ m_Stream, onFinish });
    }

    void Channel::BeforeAudioEnds(double aheadSeconds, const std::function<void(double)>& onFinish)
    {
        double position = Length() - aheadSeconds;
        size_t positionBytes = BASS_ChannelSeconds2Bytes(m_Stream, position);
        BASS_ChannelSetSync(m_Stream, BASS_SYNC_POS, positionBytes, BeforeAudioEndsSyncCallFunction, (void*)new OnSyncBeforeAudioEndsData{ m_Stream, onFinish });
    }

    void Channel::Acquire()
    {
        if (m_Status != ChannelStatus::AVAILABLE)
        {
            log->Error("Trying to acquire a non-available channel. We will allow that to not crash the game but the music may be funky.");
        }
        m_Status = ChannelStatus::ACQUIRED;
    }

    void Channel::Release()
    {
        m_Status = ChannelStatus::AVAILABLE;
        BASS_ChannelStop(m_Stream);
    }

    bool Channel::IsPlaying() const
    {
        return BASS_ChannelIsActive(m_Stream) == BASS_ACTIVE_PLAYING;
    }

    double Channel::Position() const
    {
        if (m_Stream > 0)
        {
            return BASS_ChannelBytes2Seconds(m_Stream, BASS_ChannelGetPosition(m_Stream, BASS_POS_BYTE));
        }
        return -1;
    }

    double Channel::Length() const
    {
        if (m_Stream > 0)
        {
            return BASS_ChannelBytes2Seconds(m_Stream, BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE));
        }
        return -1;
    }

    void Channel::OnPositionSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* payload = static_cast<OnSyncWhenAudioEndsData*>(userData);
        if (channel != payload->Channel) return;
        if (payload->Function) { payload->Function(); }
        else { CreateLogger("HSYNC::OnPositionSyncCallFunction")->Error("onFinish is nullptr"); }
    }

    void Channel::OnSlideVolumeSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* onFinish = static_cast<std::function<void()>*>(userData);
        if (onFinish) { (*onFinish)(); }
        else { CreateLogger("HSYNC::OnSlideVolumeSyncCallFunction")->Error("onFinish is nullptr"); }
    }

    void Channel::OnAudioEndSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* payload = static_cast<OnSyncWhenAudioEndsData*>(userData);
        if (channel != payload->Channel) return;
        if (payload->Function) { payload->Function(); }
        else { CreateLogger("HSYNC::OnAudioEndSyncCallFunction")->Error("onFinish is nullptr"); }
    }

    void Channel::BeforeAudioEndsSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* payload = static_cast<OnSyncBeforeAudioEndsData*>(userData);
        if (channel != payload->Channel) return;
        double aheadSeconds = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetLength(channel, BASS_POS_BYTE) - BASS_ChannelGetPosition(channel, BASS_POS_BYTE));
        if (payload->Function) { payload->Function(aheadSeconds); }
        else { CreateLogger("HSYNC::BeforeAudioEndsSyncCallFunction")->Error("onFinish is nullptr"); }
    }
}
