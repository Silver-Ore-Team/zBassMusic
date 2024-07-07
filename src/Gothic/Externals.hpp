#include <thread>
#include <string>
#include <sstream>

namespace GOTHIC_NAMESPACE
{
    int BassMusic_Play()
    {
        zSTRING id;
        parser->GetParameter(id);
        if (zCMusicSystem::s_musicSystemDisabled)
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::BassMusic_Play");
            log->Error("Music system disabled.");
            return 0;
        }
        zCMusicTheme* theme = zmusic->LoadThemeByScript(id);
        zmusic->PlayTheme(theme, zMUS_THEME_VOL_DEFAULT, zMUS_TR_DEFAULT, zMUS_TRSUB_DEFAULT);
        return 0;
    }

    int BassMusic_OnEndEvent()
    {
        int funcId;
        parser->GetParameter(funcId);
        Globals->Event_OnEnd_Functions.Insert(funcId);
        return 0;
    }

    int BassMusic_OnTransitionEvent()
    {
        int funcId;
        parser->GetParameter(funcId);
        Globals->Event_OnTransition_Functions.Insert(funcId);
        return 0;
    }

    int BassMusic_OnChangeEvent()
    {
        int funcId;
        parser->GetParameter(funcId);
        Globals->Event_OnChange_Functions.Insert(funcId);
        return 0;
    }

    int BassMusic_SetFullScriptControl()
    {
        int active;
        parser->GetParameter(active);
        Globals->FullScriptControl = static_cast<bool>(active);
        if (Globals->FullScriptControl)
        {
            zmusic->Stop();
        }
        return 0;
    }

    int BassMuisc_Opt_TransitionTime()
    {
        float time;
        parser->GetParameter(time);
        zoptions->WriteReal("BASSMUSIC", "TransitionTime", time, true);
        NH::Bass::Options->TransitionTime = time;
        return 0;
    }

    int BassMusic_Opt_ForceDisableReverb()
    {
        int enabled;
        parser->GetParameter(enabled);
        zoptions->WriteBool("BASSMUSIC", "ForceDisableReverb", enabled, true);
        NH::Bass::Options->ForceDisableReverb = enabled;
        return 0;
    }

    int BassMusic_Opt_ForceFadeTransition()
    {
        int enabled;
        parser->GetParameter(enabled);
        zoptions->WriteBool("BASSMUSIC", "ForceFadeTransition", enabled, true);
        NH::Bass::Options->ForceDisableReverb = enabled;
        return 0;
    }

    // func void BassMusic_AddMidiFile(var string theme, var string filter, var midiFilename)
    int BassMusic_AddMidiFile()
    {
        static NH::Logger* log = NH::CreateLogger("zBassMusic::AddMidiFile");

        zSTRING theme, filter, midiFilename;
        parser->GetParameter(midiFilename);
        parser->GetParameter(filter);
        parser->GetParameter(theme);

        auto target = NH::Bass::Engine::GetInstance()->GetMusicManager().GetTheme(NH::String(theme));
        if (!target)
        {
            log->Error("Theme {0} not found", theme.ToChar());
            return 0;
        }

        target->AddMidiFile(NH::String(filter), std::make_shared<NH::Bass::MidiFile>(target->GetName(), NH::String(midiFilename.ToChar())));
        return 0;
    }

    // func void BassMusic_AddTransitionTimePoint(var string theme, var string filter, var float start, var float duration, var int effect, var float nextStart, var float nextDuration, var int nextEffect)
    int BassMusic_AddTransitionTimePoint()
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

        auto target = NH::Bass::Engine::GetInstance()->GetMusicManager().GetTheme(NH::String(theme.ToChar()));
        if (!target)
        {
            log->Error("Theme {0} not found", theme.ToChar());
            return 0;
        }

        NH::Bass::Transition::TimePoint tp { start, duration, (NH::Bass::TransitionEffect)effect, nextStart, nextDuration, (NH::Bass::TransitionEffect)nextEffect };
        target->GetTransitionInfo().AddTimePoint(tp, NH::String(filter.ToChar()));

        return 0;
    }

    // func void BassMusic_AddJingle(var string name, var string filter, var string jingle, var float delay)
    int BassMusic_AddJingle()
    {
        static NH::Logger* log = NH::CreateLogger("zBassMusic::AddJingle");

        zSTRING name, filter, jingle;
        float delay;
        parser->GetParameter(delay);
        parser->GetParameter(jingle);
        parser->GetParameter(filter);
        parser->GetParameter(name);

        NH::Bass::Engine::GetInstance()->GetMusicManager().AddJingle(name.ToChar(), jingle.ToChar(), delay, filter.ToChar());

        return 0;
    }

    void DefineExternalsMusic()
    {
        parserMusic->AddClassOffset(Globals->BassMusicThemeClassName, sizeof(BassMusicTheme));
        parserMusic->AddClassOffset(Globals->BassMusicThemeAudioClassName, sizeof(BassMusicThemeAudio));
        parserMusic->DefineExternal("BassMusic_AddMidiFile", BassMusic_AddMidiFile, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parserMusic->DefineExternal("BassMusic_AddTransitionTimePoint", BassMusic_AddTransitionTimePoint, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_INT, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parserMusic->DefineExternal("BassMusic_AddJingle", BassMusic_AddJingle, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
    }

    void DefineExternals()
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
        parser->DefineExternal("BassMusic_Opt_TransitionTime", BassMuisc_Opt_TransitionTime, zPAR_TYPE_VOID, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
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