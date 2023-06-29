namespace GOTHIC_NAMESPACE
{
	struct GlobalsDef
	{
		zSTRING CMusicThemeClass = "C_MUSICTHEME";
		zSTRING BassMusic_ActiveThemeFilename = "";
		zSTRING BassMusic_ActiveThemeID = "";
		zCArray<int> Event_OnEnd_Functions{};
		zCArray<int> Event_OnTransition_Functions{};
		zCArray<int> Event_OnChange_Functions{};
		bool FullScriptControl = false;
	};

	GlobalsDef* CreateGlobals()
	{
		if (GetGameVersion() != ENGINE)
		{
			return nullptr;
		}
		return new GlobalsDef();
	}

	GlobalsDef* Globals = CreateGlobals();
}
