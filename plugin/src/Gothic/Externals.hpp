namespace GOTHIC_NAMESPACE
{
	int BassMusic_Play()
	{
		zSTRING id;
		parser->GetParameter(id);
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
		NH::Bass::Options.TransitionTime = time;
		return 0;
	}

	int BassMusic_Opt_ForceDisableReverb()
	{
		int enabled;
		parser->GetParameter(enabled);
		zoptions->WriteBool("BASSMUSIC", "ForceDisableReverb", enabled, true);
		NH::Bass::Options.ForceDisableReverb = enabled;
		return 0;
	}

	int BassMusic_Opt_ForceFadeTransition()
	{
		int enabled;
		parser->GetParameter(enabled);
		zoptions->WriteBool("BASSMUSIC", "ForceFadeTransition", enabled, true);
		NH::Bass::Options.ForceDisableReverb = enabled;
		return 0;
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

		if (NH::Bass::Options.CreateMainParserCMusicTheme)
		{
			zCMusicTheme theme;
			parser->AddClassOffset(Globals->CMusicThemeClass, reinterpret_cast<int>(&theme.dScriptEnd) - reinterpret_cast<int>(&theme.fileName));
		}
	}
}