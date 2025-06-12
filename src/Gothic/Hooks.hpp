namespace GOTHIC_NAMESPACE
{
    //  G1: 0x006E5920 public: int __thiscall zCParser::Parse(class zSTRING)
    // G1A: 0x0071E400 public: int __thiscall zCParser::Parse(class zSTRING)
    //  G2: 0x0072F160 public: int __thiscall zCParser::Parse(class zSTRING)
    // G2A: 0x0078EBA0 public: int __thiscall zCParser::Parse(class zSTRING)
    inline auto* zCParser_Parse = reinterpret_cast<void*>(zSwitch(0x006E5920, 0x0071E400, 0x0072F160, 0x0078EBA0));

    //  G1: 0x00424AF0 public: void __thiscall CGameManager::Run(void)
    // G1A: 0x004275D0 public: void __thiscall CGameManager::Run(void)
    //  G2: 0x004254F0 public: void __thiscall CGameManager::Run(void)
    // G2A: 0x00425830 public: void __thiscall CGameManager::Run(void)
    inline auto* CGameManager_Run = reinterpret_cast<void*>(zSwitch(0x00424AF0, 0x004275D0, 0x004254F0, 0x00425830));

    //  G1: 0x006495B0 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
    // G1A: 0x006715F0 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
    //  G2: 0x00677A00 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
    // G2A: 0x006D4780 private: void __thiscall oCGame::DefineExternals_Ulfi(class zCParser *)
    inline auto* oCGame_DefineExternals_Ulfi = reinterpret_cast<void*>(zSwitch(0x006495B0, 0x006715F0, 0x00677A00, 0x006D4780));

    //  G1: 0x0063DBE0 public: virtual void __thiscall oCGame::Render(void)
    // G1A: 0x006648F0 public: virtual void __thiscall oCGame::Render(void)
    //  G2: 0x0066B930 public: virtual void __thiscall oCGame::Render(void)
    // G2A: 0x006C86A0 public: virtual void __thiscall oCGame::Render(void)
    inline auto* oCGame_Render = reinterpret_cast<void*>(zSwitch(0x0063DBE0, 0x006648F0, 0x0066B930, 0x006C86A0));

    //  G1: 0x004CF470 public: virtual int __thiscall zCMenu::HandleFrame(void)
    // G1A: 0x004DFD00 public: virtual int __thiscall zCMenu::HandleFrame(void)
    //  G2: 0x004D9B20 public: virtual int __thiscall zCMenu::HandleFrame(void)
    // G2A: 0x004DC1C0 public: virtual int __thiscall zCMenu::HandleFrame(void)
    inline auto* zCMenu_HandleFrame = reinterpret_cast<void*>(zSwitch(0x004CF470, 0x004DFD00, 0x004D9B20, 0x004DC1C0));

    // We are hooking at the end of the function here
    //  G1: 0x0045C9F0 (+ 0x172) public: int __thiscall zCOption::WriteString(class zSTRING const &,char const *,class zSTRING,int) - 0x0045CB62 
    // G1A: 0x00463FB0 (+ 0x156) public: int __thiscall zCOption::WriteString(class zSTRING const &,char const *,class zSTRING,int) - 0x00464106  
    //  G2: 0x004617C0 (+ 0x172) public: int __thiscall zCOption::WriteString(class zSTRING const &,char const *,class zSTRING,int) - 0x00461932 
    // G2A: 0x00461FD0 (+ 0x172) public: int __thiscall zCOption::WriteString(class zSTRING const &,char const *,class zSTRING,int) - 0x00462142 
    inline auto* zCOption_WriteString = reinterpret_cast<void*>(zSwitch(0x0045CB62, 0x00464106, 0x00461932, 0x00462142));

    void __fastcall zCParser_Parse_PartialHook(Union::Registers& reg);
    inline auto Partial_zCParser_Parse = Union::CreatePartialHook(zCParser_Parse, zCParser_Parse_PartialHook);
    inline void __fastcall zCParser_Parse_PartialHook(Union::Registers& reg)
    {
        if (const auto* p = reinterpret_cast<zCParser*>(reg.ecx); p == parserMusic)
        {
            DefineExternalsMusic();
        }
    }

    void __fastcall CGameManager_Run_PartialHook();
    inline auto Partial_CGameManager_Run = Union::CreatePartialHook(CGameManager_Run, CGameManager_Run_PartialHook);
    inline void __fastcall CGameManager_Run_PartialHook()
    {
        static NH::Logger* log = NH::CreateLogger("zBassMusic::CGameManager::Run");
        log->Info("zBassMusic {0} {1} (build: {2}, branch: {3}, revision: {4})", BUILD_VERSION, BUILD_TYPE, BUILD_TIME, BUILD_BRANCH, BUILD_REVISION);
        ApplyOptions();
        if (!zoptions->Parm("ZNOMUSIC"))
        {
            auto* bassEngine = NH::Bass::Engine::GetInstance();
            if (auto* directMusic = dynamic_cast<zCMusicSys_DirectMusic*>(zmusic))
            {
                zmusic = new CMusicSys_Bass(bassEngine, directMusic);
                UpdateMusicOptions();
                log->Info("Set music system to CMusicSys_Bass");

                BassLoader bassLoaderMusic(parserMusic);
                bassLoaderMusic.Load();

                if (NH::Bass::Options->CreateMainParserCMusicTheme)
                {
                    BassLoader bassLoaderMain(parser);
                    bassLoaderMain.Load();
                }
            }
            else
            {
                log->Fatal("Failed to initialize, dynamic_cast<zCMusicSys_DirectMusic*>(zmusic) is null\n  at {0}{1}", __FILE__, __LINE__);
            }
        }
    }

    void __fastcall DefineExternals_Ulfi_PartialHook();
    inline auto Partial_DefineExternals_Ulfi = Union::CreatePartialHook(oCGame_DefineExternals_Ulfi, &DefineExternals_Ulfi_PartialHook);
    inline void __fastcall DefineExternals_Ulfi_PartialHook()
    {
        DefineExternals();
    }

    void __fastcall oCGame_Render_PartialHook();
    inline auto Partial_oCGame_Render = Union::CreatePartialHook(oCGame_Render, oCGame_Render_PartialHook);
    inline void __fastcall oCGame_Render_PartialHook()
    {
        NH::Bass::Engine::GetInstance()->Update();
    }

    void __fastcall oCMenu_Render_PartialHook();
    inline auto Partial_oCMenu_HandleFrame = Union::CreatePartialHook(zCMenu_HandleFrame, oCMenu_Render_PartialHook);
    inline void __fastcall oCMenu_Render_PartialHook()
    {
        NH::Bass::Engine::GetInstance()->Update();
    }

    void __fastcall zCOption_WriteString_PartialHook();
    inline auto Partial_zCOption_WriteString = Union::CreatePartialHook(zCOption_WriteString, zCOption_WriteString_PartialHook);
    inline void __fastcall zCOption_WriteString_PartialHook()
    {
        UpdateMusicOptions();
    }

}