namespace GOTHIC_NAMESPACE
{
    inline int BassMusic_Play()
    {
        static NH::Logger* log = NH::CreateLogger("zBassMusic::BassMusic_Play");

        zSTRING id;
        parser->GetParameter(id);
        if (zCMusicSystem::s_musicSystemDisabled)
        {
            log->Error("Music system disabled.");
            return 0;
        }
        zCMusicTheme* theme = zmusic->LoadThemeByScript(id);
        if (theme == nullptr) {
            log->Error("Illegal argument: theme == nullptr. Check BassMusic_Play(theme) call and make sure that the theme does exist.");
            return 0;
        }
        zmusic->PlayTheme(theme, zMUS_THEME_VOL_DEFAULT, zMUS_TR_DEFAULT, zMUS_TRSUB_DEFAULT);
        return 0;
    }

    inline int BassMusic_OnEndEvent()
    {
        int funcId;
        parser->GetParameter(funcId);
        Globals->Event_OnEnd_Functions.Insert(funcId);
        return 0;
    }

    inline int BassMusic_OnTransitionEvent()
    {
        int funcId;
        parser->GetParameter(funcId);
        Globals->Event_OnTransition_Functions.Insert(funcId);
        return 0;
    }

    inline int BassMusic_OnChangeEvent()
    {
        int funcId;
        parser->GetParameter(funcId);
        Globals->Event_OnChange_Functions.Insert(funcId);
        return 0;
    }

    inline int BassMusic_SetFullScriptControl()
    {
        int active;
        parser->GetParameter(active);
        if (active)
        {
            zmusic->Stop();
        }
        Globals->FullScriptControl = static_cast<bool>(active);
        return 0;
    }

    inline int BassMusic_Opt_TransitionTime()
    {
        float time;
        parser->GetParameter(time);
        zoptions->WriteReal("BASSMUSIC", "TransitionTime", time, true);
        NH::Bass::Options->TransitionTime = time;
        return 0;
    }

    inline int BassMusic_Opt_ForceDisableReverb()
    {
        int enabled;
        parser->GetParameter(enabled);
        zoptions->WriteBool("BASSMUSIC", "ForceDisableReverb", enabled, true);
        NH::Bass::Options->ForceDisableReverb = enabled;
        return 0;
    }

    inline int BassMusic_Opt_ForceFadeTransition()
    {
        int enabled;
        parser->GetParameter(enabled);
        zoptions->WriteBool("BASSMUSIC", "ForceFadeTransition", enabled, true);
        NH::Bass::Options->ForceDisableReverb = enabled;
        return 0;
    }

    // func void BassMusic_AddMidiFile(var string theme, var string filter, var midiFilename)
    inline int BassMusic_AddMidiFile()
    {
        static NH::Logger* log = NH::CreateLogger("zBassMusic::AddMidiFile");

        zSTRING theme, filter, midiFilename;
        parser->GetParameter(midiFilename);
        parser->GetParameter(filter);
        parser->GetParameter(theme);

        const auto target = NH::Bass::Engine::GetInstance()->GetMusicManager().GetTheme(theme.ToChar());
        if (!target)
        {
            log->Error("Theme {0} not found. Check BassMusic_AddMidiFile(theme, filter, midi_filename) call and make sure that the theme does exist.", theme.ToChar());
            return 0;
        }

        target->AddMidiFile(filter.ToChar(), std::make_shared<NH::Bass::MidiFile>(target->GetName(), NH::String(midiFilename.ToChar())));
        return 0;
    }

    // func void BassMusic_AddTransitionTimePoint(var string theme, var string filter, var float start, var float duration, var int effect, var float nextStart, var float nextDuration, var int nextEffect)
    inline int BassMusic_AddTransitionTimePoint()
    {
        static NH::Logger* log = NH::CreateLogger("zBassMusic::AddTransitionTimePoint");

        zSTRING theme, filter;
        float start, duration, nextStart, nextDuration;
        int effect, nextEffect;
        parser->GetParameter(nextEffect);
        parser->GetParameter(nextDuration);
        parser->GetParameter(nextStart);
        parser->GetParameter(effect);
        parser->GetParameter(duration);
        parser->GetParameter(start);
        parser->GetParameter(filter);
        parser->GetParameter(theme);

        const auto target = NH::Bass::Engine::GetInstance()->GetMusicManager().GetTheme(theme.ToChar());
        if (!target)
        {
            log->Error("Theme {0} not found. Check BassMusic_AddTransitionTimePoint(theme, filter, start, duration, effect, next_start, next_duration, next_effect) call and make sure that the theme does exist.", theme.ToChar());
            return 0;
        }

        const NH::Bass::Transition::TimePoint tp { start, duration, static_cast<NH::Bass::TransitionEffect>(effect), nextStart, nextDuration, static_cast<NH::Bass::TransitionEffect>(nextEffect) };
        target->GetTransitionInfo().AddTimePoint(tp, filter.ToChar());

        return 0;
    }

    // func void BassMusic_AddJingle(var string name, var string filter, var string jingle, var float delay)
    inline int BassMusic_AddJingle()
    {
        zSTRING name, filter, jingle;
        float delay;
        parser->GetParameter(delay);
        parser->GetParameter(jingle);
        parser->GetParameter(filter);
        parser->GetParameter(name);

        NH::Bass::Engine::GetInstance()->GetMusicManager().AddJingle(name.ToChar(), jingle.ToChar(), delay, filter.ToChar());

        return 0;
    }

    inline void DefineExternalsMusic()
    {
        parserMusic->AddClassOffset(Globals->BassMusicThemeClassName, sizeof(BassMusicTheme));
        parserMusic->AddClassOffset(Globals->BassMusicThemeAudioClassName, sizeof(BassMusicThemeAudio));
        parserMusic->DefineExternal("BassMusic_AddMidiFile", BassMusic_AddMidiFile, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parserMusic->DefineExternal("BassMusic_AddTransitionTimePoint", BassMusic_AddTransitionTimePoint, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_INT, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parserMusic->DefineExternal("BassMusic_AddJingle", BassMusic_AddJingle, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
    }

    inline void DefineExternals()
    {
        parser->DefineExternalVar("BassMusic_ActiveThemeFilename", &Globals->BassMusic_ActiveThemeFilename, zPAR_TYPE_STRING, 1);
        parser->DefineExternalVar("BassMusic_ActiveThemeID", &Globals->BassMusic_ActiveThemeID, zPAR_TYPE_STRING, 1);
        parser->DefineExternalVar("BassMusic_EventThemeFilename", &Globals->BassMusic_EventThemeFilename, zPAR_TYPE_STRING, 1);
        parser->DefineExternalVar("BassMusic_EventThemeID", &Globals->BassMusic_EventThemeID, zPAR_TYPE_STRING, 1);

        parser->DefineExternal("BassMusic_Play", BassMusic_Play, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_OnEndEvent", BassMusic_OnEndEvent, zPAR_TYPE_VOID, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_OnTransitionEvent", BassMusic_OnTransitionEvent, zPAR_TYPE_VOID, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_OnChangeEvent", BassMusic_OnChangeEvent, zPAR_TYPE_VOID, zPAR_TYPE_FUNC, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_SetFullScriptControl", BassMusic_SetFullScriptControl, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_Opt_TransitionTime", BassMusic_Opt_TransitionTime, zPAR_TYPE_VOID, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_Opt_ForceDisableReverb", BassMusic_Opt_ForceDisableReverb, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_Opt_ForceFadeTransition", BassMusic_Opt_ForceFadeTransition, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);

        parser->DefineExternal("BassMusic_AddMidiFile", BassMusic_AddMidiFile, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_AddTransitionTimePoint", BassMusic_AddTransitionTimePoint, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_INT, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("BassMusic_AddJingle", BassMusic_AddJingle, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);

        if (NH::Bass::Options->CreateMainParserCMusicTheme)
        {
            zCMusicTheme theme;
            parser->AddClassOffset(Globals->BassMusicThemeClassName, sizeof(BassMusicTheme));
            parser->AddClassOffset(Globals->BassMusicThemeAudioClassName, sizeof(BassMusicThemeAudio));
            parser->AddClassOffset(Globals->CMusicThemeClass, reinterpret_cast<int>(&theme.dScriptEnd) - reinterpret_cast<int>(&theme.fileName));
        }
    }
}