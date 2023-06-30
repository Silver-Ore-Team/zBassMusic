namespace GOTHIC_NAMESPACE
{
	// G1:	0x004240C0 public: void __thiscall CGameManager::Init(struct HWND__ * &)
	// G2A:	0x00424C70 public: void __thiscall CGameManager::Init(struct HWND__ * &)
	auto Ivk_CGameManager_Init = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x004240C0, 0x00424C70)), &CGameManager::Init_Hook, Union::HookType::Hook_CallPatch);
	inline void CGameManager::Init_Hook(HWND__*& hwnd)
	{
		(this->*Ivk_CGameManager_Init)(hwnd);
		ApplyOptions();
		if (!zoptions->Parm("ZNOMUSIC")) {
			auto* bassEngine = NH::Bass::Engine::Initialize();
			auto* directMusic = dynamic_cast<zCMusicSys_DirectMusic*>(zmusic);
			zmusic = new CMusicSys_Bass(bassEngine, directMusic);
			NH::Log::Info("Engine_Init", "Set music system to CMusicSys_Bass");
		}
	}
}