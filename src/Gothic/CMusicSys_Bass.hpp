namespace GOTHIC_NAMESPACE
{
    namespace BassEvent
    {
        inline void Event_OnEnd(const NH::Bass::Event& event, [[maybe_unused]] void* userData)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnEnd");

            auto [theme, audioId] = std::get<NH::Bass::Event::MusicEnd>(event.Data);
            const zSTRING filename{theme->GetAudioFile(audioId).Filename.c_str()};
            const zSTRING name{theme->GetName().c_str()};
            log->Trace("{0}, {1}", name.ToChar(), filename.ToChar());

            for (int i = 0; i < Globals->Event_OnEnd_Functions.GetNumInList(); i++)
            {
                const int funcId = Globals->Event_OnEnd_Functions[i];
                Globals->BassMusic_EventThemeFilename = filename;
                Globals->BassMusic_EventThemeID = name;
                parser->CallFunc(funcId);
            }
        }

        inline void Event_OnTransition(const NH::Bass::Event& event, [[maybe_unused]] void* userData)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnTransition");

            auto [theme, audioId, timeLeft] = std::get<NH::Bass::Event::MusicTransition>(event.Data);
            const zSTRING filename{theme->GetAudioFile(audioId).Filename.c_str()};
            const zSTRING name{theme->GetName().c_str()};
            log->Trace("{0}, {1}", name.ToChar(), filename.ToChar());

            for (int i = 0; i < Globals->Event_OnTransition_Functions.GetNumInList(); i++)
            {
                const int funcId = Globals->Event_OnTransition_Functions[i];
                Globals->BassMusic_EventThemeFilename = filename;
                Globals->BassMusic_EventThemeID = name;
                parser->CallFunc(funcId, timeLeft);
            }
        }

        inline void Event_OnChange(const NH::Bass::Event& event, [[maybe_unused]] void* userData)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnChange");

            const auto [theme, audioId] = std::get<NH::Bass::Event::MusicChange>(event.Data);
            const zSTRING filename{theme->GetAudioFile(audioId).Filename.c_str()};
            const zSTRING name{ theme->GetName().c_str() };
            log->Trace("{0}, {1}", name.ToChar(), filename.ToChar());

            Globals->BassMusic_ActiveThemeFilename = filename;
            Globals->BassMusic_ActiveThemeID = name;

            for (int i = 0; i < Globals->Event_OnChange_Functions.GetNumInList(); i++)
            {
                const int funcId = Globals->Event_OnChange_Functions[i];
                Globals->BassMusic_EventThemeFilename = filename;
                Globals->BassMusic_EventThemeID = name;
                parser->CallFunc(funcId);
            }
        }
    }

    class CMusicSys_Bass final : public zCMusicSystem
    {
    private:
        NH::Logger* log = NH::CreateLogger("zBassMusic::CMusicSys_Bass");
        NH::Bass::Engine* m_BassEngine;
        zCMusicSys_DirectMusic* m_DirectMusic;
        zCMusicTheme* m_ActiveTheme = nullptr;

        static bool IsDirectMusicFormat(const zSTRING& file)
        {
            static constexpr std::string_view validExt{ ".sgt" };

            if (static_cast<uint32_t>(file.Length()) < validExt.size())
            {
                return false;
            }

            const auto fileLen = static_cast<size_t>(file.Length());
            const auto extStart = fileLen - validExt.size();
            const std::string_view fileExt{ std::next(file.ToChar(), extStart), validExt.size() };

            auto toLowerSimple = [](const char t_char)
            {
                return t_char >= 'A' && t_char <= 'Z'
                       ? t_char + ('a' - 'A')
                       : t_char;
            };

            return std::ranges::equal(fileExt | std::ranges::views::transform(toLowerSimple), validExt);
        }

    public:

        CMusicSys_Bass(NH::Bass::Engine* bassEngine, zCMusicSys_DirectMusic* directMusic)
                : m_BassEngine(bassEngine), m_DirectMusic(directMusic)
        {
            m_BassEngine->GetEM().AddSubscriber(NH::Bass::EventType::MUSIC_END, &BassEvent::Event_OnEnd, this);
            m_BassEngine->GetEM().AddSubscriber(NH::Bass::EventType::MUSIC_TRANSITION, &BassEvent::Event_OnTransition, this);
            m_BassEngine->GetEM().AddSubscriber(NH::Bass::EventType::MUSIC_CHANGE, &BassEvent::Event_OnChange, this);
        }

        zCMusicTheme* LoadThemeByScript(zSTRING const& id) override
        {
            if (s_musicSystemDisabled || id.IsEmpty())
            {
                return nullptr;
            }

            log->Trace("LoadThemeByScript: {0}", id.ToChar());

            zSTRING identifier = id;
            if (m_ActiveTheme && identifier.Upper() == m_ActiveTheme->name)
            {
                return m_ActiveTheme;
            }

            auto* theme = new zCMusicTheme;
            theme->name = identifier;

            if (!(NH::Bass::Options->CreateMainParserCMusicTheme && parser->CreateInstance(identifier, &theme->fileName)))
            {
                parserMusic->CreateInstance(identifier, &theme->fileName);
            }

            if (IsDirectMusicFormat(theme->fileName))
            {
                delete theme;
                theme = m_DirectMusic->LoadThemeByScript(id);
            }

            return theme;
        }

        void PlayThemeByScript(zSTRING const& id, int manipulate, int* done) override
        {
            if (s_musicSystemDisabled || id.IsEmpty())
            {
                return;
            }

            log->Trace("PlayThemeByScript: {0}", id.ToChar());

            if (Globals->FullScriptControl)
            {
                log->Trace("PlayThemeByScript skipped because FullScriptControl is enabled.");
                return;
            }

            static zSTRING nowPlaying = "";

            zSTRING identifier = id;
            identifier.Upper();
            if (m_ActiveTheme && identifier.Upper() == m_ActiveTheme->name || nowPlaying == identifier)
            {
                return;
            }

            zCMusicTheme* theme = LoadThemeByScript(id);
            if (theme && IsDirectMusicFormat(theme->fileName))
            {
                nowPlaying = "";
                m_ActiveTheme = theme;
                m_BassEngine->StopMusic();
                return m_DirectMusic->PlayThemeByScript(id, manipulate, done);
            }

            m_DirectMusic->Stop();
            nowPlaying = identifier;
            m_ActiveTheme = nullptr;
            m_BassEngine->GetCommandQueue().AddCommand(std::make_shared<NH::Bass::ChangeZoneCommand>(identifier.ToChar()));

            if (done)
            {
                *done = true;
            }
        }

        void PlayTheme(zCMusicTheme* theme, float const& themeVolume, zTMus_TransType const& transition, zTMus_TransSubType const& subTransition) override
        {
            if (theme == nullptr) {
                log->Error("PlayTheme illegal argument: theme == nullptr");
                return;
            }

            log->Trace("PlayTheme: {0}", theme->fileName.ToChar());

            if (IsDirectMusicFormat(theme->fileName))
            {
                m_BassEngine->StopMusic();
                m_ActiveTheme = theme;
                m_DirectMusic->PlayTheme(theme, themeVolume, transition, subTransition);
                return;
            }

            // Called from an external
            m_DirectMusic->Stop();
            m_ActiveTheme = theme;
            zSTRING identifier = theme->name;
            identifier.Upper();
            m_BassEngine->GetCommandQueue().AddCommand(std::make_shared<NH::Bass::ScheduleThemeChangeCommand>(identifier.ToChar()));
        }

        zCMusicTheme* GetActiveTheme() override
        {
            return m_ActiveTheme;
        }

        void DoMusicUpdate() override
        {
            m_DirectMusic->DoMusicUpdate();
        }

        void Mute() override
        {
            log->Trace("Mute");
            m_BassEngine->SetVolume(0.0f);
            m_DirectMusic->Mute();
        }

        void Stop() override
        {
            log->Trace("Stop");
            m_ActiveTheme = nullptr;
            m_DirectMusic->Stop();
        }

        [[nodiscard]] float GetVolume() const override
        {
            return m_BassEngine->GetVolume();
        }

        void SetVolume(const float masterVolume) override
        {
            m_BassEngine->SetVolume(masterVolume);
            m_DirectMusic->SetVolume(masterVolume);
        }

        int IsAvailable(zSTRING const& id) override
        {
            return (NH::Bass::Options->CreateMainParserCMusicTheme && parser->GetSymbol(id) != nullptr) || parserMusic->GetSymbol(id) != nullptr;
        }
    };
}