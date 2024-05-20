namespace GOTHIC_NAMESPACE
{
	//  G1: 0x00424AF0 public: void __thiscall CGameManager::Run(void)
	// G1A: 0x004275D0 public: void __thiscall CGameManager::Run(void)
	//  G2: 0x004254F0 public: void __thiscall CGameManager::Run(void)
	// G2A: 0x00425830 public: void __thiscall CGameManager::Run(void)
	void* CGameManager_Run = (void*)zSwitch(0x00424AF0, 0x004275D0, 0x004254F0, 0x00425830);

	//  G1: 0x006495B0 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
	// G1A: 0x006715F0 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
	//  G2: 0x00677A00 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
	// G2A: 0x006D4780 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
	void* oCGame_DefineExternals_Ulfi = (void*)zSwitch(0x006495B0, 0x006715F0, 0x00677A00, 0x006D4780);

	//  G1: 0x0063DBE0 public: virtual void __thiscall oCGame::Render(void)
	// G1A: 0x006648F0 public: virtual void __thiscall oCGame::Render(void)
	//  G2: 0x0066B930 public: virtual void __thiscall oCGame::Render(void)
	// G2A: 0x006C86A0 public: virtual void __thiscall oCGame::Render(void)
	void* oCGame_Render = (void*)zSwitch(0x0063DBE0, 0x006648F0, 0x0066B930, 0x006C86A0);

	//  G1: 0x004CF470 public: virtual int __thiscall zCMenu::HandleFrame(void)
	// G1A: 0x004DFD00 public: virtual int __thiscall zCMenu::HandleFrame(void)
	//  G2: 0x004D9B20 public: virtual int __thiscall zCMenu::HandleFrame(void)
	// G2A: 0x004DC1C0 public: virtual int __thiscall zCMenu::HandleFrame(void)
	void* zCMenu_HandleFrame = (void*)zSwitch(0x004CF470, 0x004DFD00, 0x004D9B20, 0x004DC1C0);

	void __fastcall CGameManager_Run_PartialHook();
	auto Partial_CGameManager_Run = Union::CreatePartialHook(CGameManager_Run, CGameManager_Run_PartialHook);
	void __fastcall CGameManager_Run_PartialHook()
	{
		static NH::Logger* log = NH::CreateLogger("zBassMusic::CGameManager::Run");
		log->Info("zBassMusic {0} {1} (build: {2}, branch: {3}, revision: {4})", BUILD_VERSION, BUILD_TYPE, BUILD_TIME, BUILD_BRANCH, BUILD_REVISION);
		ApplyOptions();
		if (!zoptions->Parm("ZNOMUSIC"))
		{
			auto* bassEngine = NH::Bass::Engine::GetInstance();
			auto* directMusic = dynamic_cast<zCMusicSys_DirectMusic*>(zmusic);
			if (directMusic)
			{
				zmusic = new CMusicSys_Bass(bassEngine, directMusic);
				float volume = zoptions->ReadReal("SOUND", "musicVolume", 1.0f);
				zmusic->SetVolume(volume);
				log->Info("Set music system to CMusicSys_Bass");
			}
			else
			{
				log->Fatal("Failed to initialize, dynamic_cast<zCMusicSys_DirectMusic*>(zmusic) is null\n  at {0}{1}", __FILE__, __LINE__);
			}
		}
	}

	void __fastcall DefineExternals_Ulfi_PartialHook(Union::Registers& reg);
	auto Partial_DefineExternals_Ulfi = Union::CreatePartialHook(oCGame_DefineExternals_Ulfi, &DefineExternals_Ulfi_PartialHook);
	void __fastcall DefineExternals_Ulfi_PartialHook(Union::Registers& reg)
	{
		DefineExternals();
	}

 	void __fastcall oCGame_Render_PartialHook(Union::Registers& reg);
	auto Partial_oCGame_Render = Union::CreatePartialHook(oCGame_Render, oCGame_Render_PartialHook);
 	void __fastcall oCGame_Render_PartialHook(Union::Registers& reg)
	 {
		NH::Bass::Engine::GetInstance()->Update();
	}

	void __fastcall oCMenu_Render_PartialHook(Union::Registers& reg);
	auto Partial_oCMenu_HandleFrame = Union::CreatePartialHook(zCMenu_HandleFrame, oCMenu_Render_PartialHook);
	void __fastcall oCMenu_Render_PartialHook(Union::Registers& reg)
	{
		NH::Bass::Engine::GetInstance()->Update();
	}

}