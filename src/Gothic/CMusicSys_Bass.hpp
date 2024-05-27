namespace GOTHIC_NAMESPACE
{
    namespace BassEvent
    {
        void Event_OnEnd(const NH::Bass::Event& event, void* userData)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnEnd");

            NH::Bass::Event::MusicEnd data = std::get<NH::Bass::Event::MusicEnd>(event.Data);
            zSTRING filename{ data.Theme->GetAudioFile(data.AudioId).Filename.ToChar() };
            zSTRING name{ data.Theme->GetName() };
            log->Trace("{0}, {1}", name.ToChar(), filename.ToChar());

            for (int i = 0; i < Globals->Event_OnEnd_Functions.GetNumInList(); i++)
            {
                const int funcId = Globals->Event_OnEnd_Functions[i];
                Globals->BassMusic_EventThemeFilename = filename;
                Globals->BassMusic_EventThemeID = name;
                parser->CallFunc(funcId);
            }
        }

        void Event_OnTransition(const NH::Bass::Event& event, void* userData)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnTransition");

            NH::Bass::Event::MusicTransition data = std::get<NH::Bass::Event::MusicTransition>(event.Data);
            zSTRING filename{ data.Theme->GetAudioFile(data.AudioId).Filename.ToChar() };
            zSTRING name{ data.Theme->GetName() };
            float timeLeft = data.TimeLeft;
            log->Trace("{0}, {1}", name.ToChar(), filename.ToChar());

            for (int i = 0; i < Globals->Event_OnTransition_Functions.GetNumInList(); i++)
            {
                const int funcId = Globals->Event_OnTransition_Functions[i];
                Globals->BassMusic_EventThemeFilename = filename;
                Globals->BassMusic_EventThemeID = name;
                parser->CallFunc(funcId, timeLeft);
            }
        }

        void Event_OnChange(const NH::Bass::Event& event, void* userData)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnChange");

            NH::Bass::Event::MusicChange data = std::get<NH::Bass::Event::MusicChange>(event.Data);
            zSTRING filename{ data.Theme->GetAudioFile(data.AudioId).Filename.ToChar() };
            zSTRING name{ data.Theme->GetName() };
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

    class CMusicSys_Bass : public zCMusicSystem
    {
    private:
        NH::Logger* log = NH::CreateLogger("zBassMusic::CMusicSys_Bass");
        NH::Bass::Engine* m_BassEngine;
        zCMusicSys_DirectMusic* m_DirectMusic;
        zCMusicTheme* m_ActiveTheme = nullptr;

        static bool IsDirectMusicFormat(const zSTRING& file)
        {
            static constexpr std::string_view validExt{ ".sgt" };

            if (file.Length() < validExt.size())
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

            return std::ranges::equal(fileExt | std::views::transform(toLowerSimple), validExt);
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

            zCMusicTheme* theme = new zCMusicTheme;
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

            zSTRING identifier = id;
            if (m_ActiveTheme && identifier.Upper() == m_ActiveTheme->name)
            {
                return;
            }

            zCMusicTheme* theme = LoadThemeByScript(id);
            if (theme && IsDirectMusicFormat(theme->fileName))
            {
                m_ActiveTheme = theme;
                m_BassEngine->StopMusic();
                return m_DirectMusic->PlayThemeByScript(id, manipulate, done);
            }

            identifier.Upper();
            m_BassEngine->GetCommandQueue().AddCommand(std::make_shared<NH::Bass::ChangeZoneCommand>(identifier.ToChar()));

            if (done)
            {
                *done = true;
            }
        }

        void PlayTheme(zCMusicTheme* theme,
                       float const& volume = zMUS_THEME_VOL_DEFAULT,
                       zTMus_TransType const& transition = zMUS_TR_DEFAULT,
                       zTMus_TransSubType const& subTransition = zMUS_TRSUB_DEFAULT) override
        {
            log->Trace("PlayTheme: {0}", theme->fileName.ToChar());

            if (IsDirectMusicFormat(theme->fileName))
            {
                m_BassEngine->StopMusic();
                m_ActiveTheme = theme;
                m_DirectMusic->PlayTheme(theme, volume, transition, subTransition);
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

        float GetVolume() const override
        {
            return m_BassEngine->GetVolume();
        }

        void SetVolume(float volume) override
        {
            m_BassEngine->SetVolume(volume);
            m_DirectMusic->SetVolume(volume);
        }

        int IsAvailable(zSTRING const& id) override
        {
            return (NH::Bass::Options->CreateMainParserCMusicTheme && parser->GetSymbol(id) != nullptr) || parserMusic->GetSymbol(id) != nullptr;
        }
    };
}