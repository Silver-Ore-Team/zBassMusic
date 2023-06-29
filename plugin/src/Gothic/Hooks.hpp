namespace GOTHIC_NAMESPACE
{
	// G1:  0x00544340 public: static int __cdecl zCEngine::Init(struct HWND__ * *)
	// G2A: 0x00558BE0 public: static int __cdecl zCEngine::Init(struct HWND__ * *)
	int Engine_Init(HWND__**);
	inline auto Ivk_zCEngine_Init = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x00544340, 0x00558BE0)), &Engine_Init);

	inline int Engine_Init(HWND__** hwnd)
	{
		int result = Ivk_zCEngine_Init(hwnd);
		if (!zoptions->Parm("ZNOMUSIC")) {
			auto* bassEngine = NH::Bass::Engine::Initialize();
			auto* directMusic = dynamic_cast<zCMusicSys_DirectMusic*>(zmusic);
			zmusic = new CMusicSys_Bass(bassEngine, directMusic);
			NH::Log::Info("Engine_Init", "Set music system to CMusicSys_Bass");
		}
		return result;
	}

}