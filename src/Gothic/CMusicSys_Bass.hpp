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
            identifier.Upper();

            std::shared_ptr<NH::Bass::MusicTheme> theme = m_BassEngine->GetMusicManager().GetTheme(identifier.ToChar());
            if (!theme)
            {
                log->Error("LoadThemeByScript: Theme not found: {0}", id.ToChar());
                return nullptr;
            }
            if (IsDirectMusicFormat(theme->GetAudioFile(NH::Bass::AudioFile::DEFAULT).Filename.c_str()))
            {
                if (!parserMusic->GetSymbol(identifier) && parser->GetSymbol(identifier))
                {
                    log->Error("LoadThemeByScript: Theme: {0} uses DirectMusic format and have to be defined in the music parser", id.ToChar());
                    return nullptr;
                }
                return m_DirectMusic->LoadThemeByScript(identifier);
            }
            else 
            {
                auto* dmTheme = new zCMusicTheme;
                dmTheme->name = theme->GetName().c_str();
                if (!(NH::Bass::Options->CreateMainParserCMusicTheme && parser->CreateInstance(identifier, &dmTheme->fileName)))
                {
                    parserMusic->CreateInstance(identifier, &dmTheme->fileName);
                }
                return dmTheme;
            }
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

            zSTRING identifier = id;
            identifier.Upper();
            if (m_DirectMusic->GetActiveTheme() && m_DirectMusic->GetActiveTheme()->name == identifier)
            {
                return;
            }

            if (m_BassEngine->GetActiveTheme() && m_BassEngine->GetActiveTheme()->GetName() == identifier.ToChar())
            {
                return;
            }

            std::shared_ptr<NH::Bass::MusicTheme> theme = m_BassEngine->GetMusicManager().GetTheme(identifier.ToChar());
            if (theme && IsDirectMusicFormat(theme->GetAudioFile(NH::Bass::AudioFile::DEFAULT).Filename.c_str()))
            {
                m_BassEngine->StopMusic();
                return m_DirectMusic->PlayThemeByScript(id, manipulate, done);
            }

            m_DirectMusic->Stop();
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
                m_DirectMusic->PlayTheme(theme, themeVolume, transition, subTransition);
                return;
            }

            // Called from an external
            m_DirectMusic->Stop();
            zSTRING identifier = theme->name;
            identifier.Upper();
            m_BassEngine->GetCommandQueue().AddCommand(std::make_shared<NH::Bass::ScheduleThemeChangeCommand>(identifier.ToChar()));
        }

        zCMusicTheme* GetActiveTheme() override
        {
            if (m_BassEngine->GetActiveTheme())
            {
                auto* theme = new zCMusicTheme();
                theme->name = m_BassEngine->GetCurrentZone().c_str();
                return theme;
            }
            return m_DirectMusic->GetActiveTheme();
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
            m_DirectMusic->Stop();
            m_BassEngine->StopMusic();
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
            return (NH::Bass::Options->CreateMainParserCMusicTheme && parser->GetSymbol(id) != nullptr) || parserMusic->GetSymbol(id) != nullptr  || m_BassEngine->GetMusicManager().GetThemesForZone(id.ToChar()).size() > 0;
        }
    };
}