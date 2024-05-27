#pragma once

#include "NH/Bass/CommonTypes.h"
#include "EventManager.h"
#include "NH/Logger.h"
#include <NH/Bass/MusicTheme.h>
#include <NH/Bass/IChannel.h>

#include <functional>


namespace NH::Bass
{
    enum class ChannelStatus
    {
        AVAILABLE,
        PLAYING,
        FADING_OUT
    };

    class Channel : public IChannel
    {
        NH::Logger* log;
        ChannelStatus m_Status = ChannelStatus::AVAILABLE;
        HSTREAM m_Stream = 0;

    public:
        explicit Channel(size_t index)
        {
            log = NH::CreateLogger(Union::String::Format("zBassMusic::Channel({0})", index));
        };

        Result<void> PlayInstant(const AudioFile& audioFile) override;
        void StopInstant() override;
        void SetVolume(float volume) override;
        void SlideVolume(float targetVolume, uint32_t time) override;
        void SlideVolume(float targetVolume, uint32_t time, const std::function<void()>& onFinish) override;
        void SetDX8ReverbEffect(float reverbMix, float reverbTime, float inputGain, float highFreqRTRatio) override;

        void WhenAudioEnds(const std::function<void()>& onFinish) override;
        void BeforeAudioEnds(double aheadSeconds, const std::function<void(double)>& onFinish) override;

        [[nodiscard]] double Position() const override;
        [[nodiscard]] double Length() const override;

        void Acquire() override;
        void Release() override;
        bool IsAvailable() override
        {
            return m_Status == ChannelStatus::AVAILABLE;
        };

    private:
        static void CALLBACK OnSlideVolumeSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData);
        static void CALLBACK OnAudioEndSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData);
        static void CALLBACK BeforeAudioEndsSyncCallFunction(HSYNC, DWORD channel, DWORD data, void* userData);
    };
}
