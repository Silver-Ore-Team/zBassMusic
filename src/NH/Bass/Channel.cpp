#include "Channel.h"
#include "NH/Bass/Engine.h"
#include "Options.h"

namespace NH::Bass
{
    void Channel::Play(const std::shared_ptr<MusicTheme>& theme, HashString id)
    {
        if (m_Stream > 0)
        {
            BASS_ChannelFree(m_Stream);
        }

        m_Theme = theme;
        m_AudioId = id;
        const auto& file = theme->GetAudioFile(id);
        const auto& effects = theme->GetAudioEffects(id);

        m_Stream = BASS_StreamCreateFile(true, file.Buffer.data(), 0, file.Buffer.size(), 0);
        if (!m_Stream)
        {
            log->Error("Could not create stream: {0}\n  error: {1}\n  at {2}:{3}",
                       m_Theme->GetName(), Engine::ErrorCodeToString(BASS_ErrorGetCode()),
                       __FILE__, __LINE__);
            return;
        }

        BASS_ChannelStart(m_Stream);
        log->Info("Channel started: {0}", m_Theme->GetName());

        float targetVolume = effects.Volume.Active ? effects.Volume.Volume : 1.0f;
        BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_VOL, targetVolume);

        if (effects.FadeIn.Active)
        {
            BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_VOL, 0.0f);
            BASS_ChannelSlideAttribute(m_Stream, BASS_ATTRIB_VOL, targetVolume, effects.FadeIn.Duration);
        }

        if (effects.Loop.Active)
        {
            BASS_ChannelFlags(m_Stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
            log->Trace("Loop set {0}", m_Theme->GetName());
        }

        if (!Options->ForceDisableReverb && effects.ReverbDX8.Active)
        {
            HFX fx = BASS_ChannelSetFX(m_Stream, BASS_FX_DX8_REVERB, 1);
            BASS_DX8_REVERB params{ 0, effects.ReverbDX8.Mix, effects.ReverbDX8.Time, 0.001f };
            if (!BASS_FXSetParameters(fx, (void*)&params))
            {
                log->Error("Could not set reverb FX: {0}\n  error: {1}\n  at {2}:{3}",
                           m_Theme->GetName(), Engine::ErrorCodeToString(BASS_ErrorGetCode()),
                           __FILE__, __LINE__);
            }
            log->Trace("Reverb set: {0}", m_Theme->GetName());
        }

        if (effects.FadeOut.Active)
        {
            const QWORD length = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
            const QWORD transitionBytes = BASS_ChannelSeconds2Bytes(m_Stream, effects.FadeOut.Duration / 1000.0f);
            const QWORD offset = length - transitionBytes;
            BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, offset, OnVolumeSlideSync, this);
            log->Trace("SyncEnd set: {0}", m_Theme->GetName());
        }

        BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, 0, OnEndSync, this);
        log->Trace("SyncEnd set: {0}", m_Theme->GetName());

        m_Status = ChannelStatus::PLAYING;
        m_EventManager.DispatchEvent(MusicChangeEvent(m_Theme, m_AudioId));
    }

    double Channel::CurrentPosition() const
    {
        if (m_Stream > 0)
        {
            return BASS_ChannelBytes2Seconds(m_Stream, BASS_ChannelGetPosition(m_Stream, BASS_POS_BYTE));
        }
        return -1;
    }

    double Channel::CurrentLength() const
    {
        if (m_Stream > 0)
        {
            return BASS_ChannelBytes2Seconds(m_Stream, BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE));
        }
        return -1;
    }

    void Channel::Stop()
    {
        if (m_Stream > 0)
        {
            const auto& effects = m_Theme->GetAudioEffects(m_AudioId);
            if (effects.FadeOut.Active)
            {
                BASS_ChannelSlideAttribute(m_Stream, BASS_ATTRIB_VOL, 0.0f, effects.FadeOut.Duration);
                BASS_ChannelSetSync(m_Stream, BASS_SYNC_SLIDE, 0, OnVolumeSlideSync, this);
                m_Status = ChannelStatus::FADING_OUT;
            }
            else
            {
                BASS_ChannelFree(m_Stream);
                m_Stream = 0;
                m_Status = ChannelStatus::AVAILABLE;
            }
        }
    }

    std::shared_ptr<MusicTheme> Channel::CurrentTheme() const
    {
        return m_Theme;
    }

    const AudioFile& Channel::CurrentAudioFile() const
    {
        return m_Theme->GetAudioFile(m_AudioId);
    }

    const AudioEffects& Channel::CurrentAudioEffects() const
    {
        return m_Theme->GetAudioEffects(m_AudioId);
    }

    void Channel::OnTransitionSync(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* _this = static_cast<Channel*>(userData);
        if (_this->m_Stream == channel)
        {
            _this->m_EventManager.DispatchEvent(MusicTransitionEvent(_this->m_Theme, _this->m_AudioId, _this->CurrentAudioEffects().FadeOut.Duration));
        }
    }

    void Channel::OnEndSync(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* _this = static_cast<Channel*>(userData);
        if (_this->m_Stream == channel)
        {
            const auto& effects = _this->m_Theme->GetAudioEffects(_this->m_AudioId);
            if (!effects.Loop.Active)
            {
                _this->m_Status = ChannelStatus::AVAILABLE;
            }

            _this->m_EventManager.DispatchEvent(MusicEndEvent(_this->m_Theme, _this->m_AudioId));
        }
    }

    void Channel::OnVolumeSlideSync(HSYNC, DWORD channel, DWORD data, void* userData)
    {
        auto* _this = static_cast<Channel*>(userData);
        if (_this->m_Stream == channel && _this->m_Status == ChannelStatus::FADING_OUT)
        {
            float volume;
            BASS_ChannelGetAttribute(channel, BASS_ATTRIB_VOL, &volume);
            if (volume < DBL_EPSILON)
            {
                BASS_ChannelFree(channel);
                _this->m_Stream = 0;
                _this->m_Status = ChannelStatus::AVAILABLE;
            }
        }
    }
}
