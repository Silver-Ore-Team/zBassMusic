namespace GOTHIC_NAMESPACE
{
	// G1:	0x004240C0 public: void __thiscall CGameManager::Init(struct HWND__ * &)
	// G2A:	0x00424C70 public: void __thiscall CGameManager::Init(struct HWND__ * &)
	auto Ivk_CGameManager_Init = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004240C0, 0x00424C70)), &CGameManager::Init_Hook, Union::HookType::Hook_CallPatch);
	inline void CGameManager::Init_Hook(HWND__*& hwnd)
	{
		if (GetGameVersion() != ENGINE)
		{
			return;
		}

		(this->*Ivk_CGameManager_Init)(hwnd);

		ApplyOptions();

		if (!zoptions->Parm("ZNOMUSIC")) {
			auto* bassEngine = NH::Bass::Engine::GetInstance();
			auto* directMusic = dynamic_cast<zCMusicSys_DirectMusic*>(zmusic);
			if (directMusic) {
				zmusic = new CMusicSys_Bass(bassEngine, directMusic);
				float volume = zoptions->ReadReal("SOUND", "musicVolume", 1.0f);
				zmusic->SetVolume(volume);
				NH::Log::Info("CGameManager::Init_Hook", "Set music system to CMusicSys_Bass");
			}
			else {
				NH::Log::Error("CGameManager::Init_Hook", "Failed to initialize");
			}
		}
	}
}