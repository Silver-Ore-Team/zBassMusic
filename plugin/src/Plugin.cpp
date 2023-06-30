// Disable macro redefinition warning 
#pragma warning(disable: 4005)

#include <NH/BassOptions.h>
#include <NH/Union.h>
#include <NH/Bass.h>
#include <ZenGin/zGothicAPI.h>

#ifdef __G1
#define GOTHIC_NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
#include "Plugin.hpp"
#endif

#ifdef __G2A
#define GOTHIC_NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
#include "Plugin.hpp"
#endif

NH::Bass::BassOptions* NH::Bass::Options = new NH::Bass::BassOptions{};

EXTERN_C_START
__declspec(dllexport) void Game_DefineExternals() {
	NH::Bass::Options->LoggerLevelUnion = "DEBUG";
	NH::Bass::Options->LoggerLevelZSpy = "DEBUG";

	if (GetGameVersion() == Engine_G1)
	{
		Gothic_I_Classic::ApplyOptions();
		if (!Gothic_I_Classic::zoptions->Parm("ZNOMUSIC")) {
			auto* bassEngine = NH::Bass::Engine::Initialize();
			auto* directMusic = dynamic_cast<Gothic_I_Classic::zCMusicSys_DirectMusic*>(Gothic_I_Classic::zmusic);
			directMusic->Stop();
			Gothic_I_Classic::zmusic = new Gothic_I_Classic::CMusicSys_Bass(bassEngine, directMusic);
			NH::Log::Info("ENTRY", "Set music system to CMusicSys_Bass");
		}
		Gothic_I_Classic::DefineExternals();
	}
	if (GetGameVersion() == Engine_G2A)
	{
		Gothic_II_Addon::ApplyOptions();
		if (!Gothic_II_Addon::zoptions->Parm("ZNOMUSIC")) {
			auto* bassEngine = NH::Bass::Engine::Initialize();
			auto* directMusic = dynamic_cast<Gothic_II_Addon::zCMusicSys_DirectMusic*>(Gothic_II_Addon::zmusic);
			directMusic->Stop();
			Gothic_II_Addon::zmusic = new Gothic_II_Addon::CMusicSys_Bass(bassEngine, directMusic);
			NH::Log::Info("ENTRY", "Set music system to CMusicSys_Bass");
		}
		Gothic_II_Addon::DefineExternals();
	}
}
EXTERN_C_END