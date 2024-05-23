#pragma once

#include "NH/Bass/CommonTypes.h"
#include "EventManager.h"
#include "NH/Logger.h"

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
        MusicDef m_Music{};

    public:
        explicit Channel(size_t index, EventManager& em) : m_Index(index), m_EventManager(em)
        {
            log = NH::CreateLogger(Union::String::Format("zBassMusic::Channel({0})", index));
        };

        void Play(const MusicDef& music, const MusicFile* file);

        void Stop();

        bool IsAvailable()
        {
            return m_Status == ChannelStatus::AVAILABLE;
        };

        double CurrentPosition() const;

        double CurrentLength() const;

        const MusicDef& CurrentMusic() const;

        static void CALLBACK OnTransitionSync(HSYNC, DWORD channel, DWORD data, void* userData);

        static void CALLBACK OnEndSync(HSYNC, DWORD channel, DWORD data, void* userData);

        static void CALLBACK OnVolumeSlideSync(HSYNC, DWORD channel, DWORD data, void* userData);

    private:
    };
}
