#pragma once

#include "CommonTypes.h"
#include "EventManager.h"
#include "Channel.h"
#include "TransitionScheduler.h"
#include "NH/Logger.h"
#include "NH/HashString.h"
#include "NH/Bass/MusicManager.h"
#include "NH/Bass/Command.h"
#include <vector>
#include <mutex>
#include <chrono>
#include <unordered_map>

namespace NH::Bass
{
    class ChangeZoneCommand;
    class PlayThemeCommand;
    class ScheduleThemeChangeCommand;

    class Engine
    {
        friend class ChangeZoneCommand;
        friend class PlayThemeCommand;
        friend class ScheduleThemeChangeCommand;

        static NH::Logger* log;
        static Engine* s_Instance;
        bool m_Initialized = false;
        float m_MasterVolume = 1.0f;
        std::vector<std::shared_ptr<Channel>> m_Channels;
        std::shared_ptr<Channel> m_ActiveChannel = nullptr;
        CommandQueue m_CommandQueue{};
        EventManager m_EventManager{};
        MusicManager m_MusicManager{};
        TransitionScheduler m_TransitionScheduler{};
        std::unordered_map<HashString, MusicDef> m_MusicDefs;
        std::unordered_map<HashString, MusicFile> m_MusicFiles;

        std::mutex m_PlayMusicMutex;
        std::vector<MusicDefRetry> m_PlayMusicRetryList;

    public:
        static Engine* GetInstance();

        void Update();

        void SetVolume(float volume);

        [[nodiscard]] float GetVolume() const;

        EventManager& GetEM();

        TransitionScheduler& GetTransitionScheduler();

        MusicManager& GetMusicManager();

        CommandQueue& GetCommandQueue();

        void StopMusic();

    private:
        Engine();

        std::shared_ptr<Channel> FindAvailableChannel();

    public:
        static Union::StringUTF8 ErrorCodeToString(int code);
    };

    class ChangeZoneCommand : public Command
    {
        static Logger* log;
        HashString m_Zone;
    public:
        explicit ChangeZoneCommand(HashString zone) : m_Zone(zone) {};
        CommandResult Execute(Engine& engine) override;
    };

    class PlayThemeCommand : public Command
    {
        static Logger* log;
        HashString m_ThemeId;
        HashString m_AudioId;
        size_t m_RetryCount = 0;
    public:
        explicit PlayThemeCommand(HashString themeId, HashString audioId) : m_ThemeId(themeId), m_AudioId(audioId) {};
        CommandResult Execute(Engine& engine) override;
    };

    class ScheduleThemeChangeCommand : public Command
    {
        static Logger* log;
        HashString m_ThemeId;
    public:
        explicit ScheduleThemeChangeCommand(HashString themeId) : m_ThemeId(themeId) {};
        CommandResult Execute(Engine& engine) override;
    };
}
