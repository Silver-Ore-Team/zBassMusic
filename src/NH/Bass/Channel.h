#pragma once

#include "NH/Bass/CommonTypes.h"
#include "EventManager.h"
#include "NH/Logger.h"
#include <NH/Bass/MusicTheme.h>

#include <functional>


namespace NH::Bass
{
    enum class ChannelStatus
    {
        AVAILABLE,
        PLAYING,
        FADING_OUT
    };

    class Channel
    {
        NH::Logger* log;
        size_t m_Index;
        EventManager& m_EventManager;
        ChannelStatus m_Status = ChannelStatus::AVAILABLE;
        HSTREAM m_Stream = 0;
        std::shared_ptr<MusicTheme> m_Theme = std::shared_ptr<MusicTheme>();
        HashString m_AudioId{""};

    public:
        explicit Channel(size_t index, EventManager& em) : m_Index(index), m_EventManager(em)
        {
            log = NH::CreateLogger(Union::String::Format("zBassMusic::Channel({0})", index));
        };

        void Play(const std::shared_ptr<MusicTheme>&, HashString id = AudioFile::DEFAULT);

        void Stop();

        bool IsAvailable()
        {
            return m_Status == ChannelStatus::AVAILABLE;
        };

        [[nodiscard]] double CurrentPosition() const;

        [[nodiscard]] double CurrentLength() const;

        [[nodiscard]] std::shared_ptr<MusicTheme> CurrentTheme() const;

        [[nodiscard]] const AudioFile& CurrentAudioFile() const;

        [[nodiscard]] const AudioEffects& CurrentAudioEffects() const;

        static void CALLBACK OnTransitionSync(HSYNC, DWORD channel, DWORD data, void* userData);

        static void CALLBACK OnEndSync(HSYNC, DWORD channel, DWORD data, void* userData);

        static void CALLBACK OnVolumeSlideSync(HSYNC, DWORD channel, DWORD data, void* userData);

    private:
    };
}
