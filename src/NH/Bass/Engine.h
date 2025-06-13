#pragma once

#include "EventManager.h"
#include "Channel.h"
#include "IEngine.h"
#include "MusicManager.h"
#include "Command.h"
#include "NH/Logger.h"

#include <vector>

namespace NH::Bass
{
    class ChangeZoneCommand;
    class ScheduleThemeChangeCommand;

    class Engine final : public IEngine
    {
        friend class ChangeZoneCommand;
        friend class ScheduleThemeChangeCommand;

        static Logger* log;
        static Engine* s_Instance;
        bool m_Initialized = false;
        float m_MasterVolume = 1.0f;
        std::string m_CurrentZone;
        std::vector<std::shared_ptr<Channel>> m_Channels;
        std::shared_ptr<MusicTheme> m_ActiveTheme = nullptr;
        CommandQueue m_CommandQueue{};
        EventManager m_EventManager{};
        MusicManager m_MusicManager{};

    public:
        static Engine* GetInstance();

        void Update();
        void StopMusic();
        void SetVolume(float volume);
        [[nodiscard]] float GetVolume() const;

        std::shared_ptr<MusicTheme> GetActiveTheme();
        void SetActiveTheme(const std::shared_ptr<MusicTheme>& theme);

        std::shared_ptr<IChannel> AcquireFreeChannel() override;
        void ReleaseChannel(const std::shared_ptr<IChannel>& channel) override;

        EventManager& GetEM() override;
        MusicManager& GetMusicManager() override;
        CommandQueue& GetCommandQueue() override;
        std::string GetCurrentZone() override;
        void SetCurrentZone(const std::string& zone) override;

    private:
        Engine();

    public:
        static Union::StringUTF8 ErrorCodeToString(int code);
    };
}
