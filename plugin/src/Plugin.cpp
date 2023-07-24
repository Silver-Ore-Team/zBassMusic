// Disable macro redefinition warning 
#pragma warning(disable: 4005)

#include <NH/BassOptions.h>
#include <NH/Union.h>
#include <NH/Bass.h>
#include <Union/Hook.h>
#include <ZenGin/zGothicAPI.h>

#ifdef __G1
#define GOTHIC_NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
HOOKSPACE(Gothic_I_Classic, GetGameVersion() == ENGINE);
#include "Plugin.hpp"
#endif

#ifdef __G2A
#define GOTHIC_NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
HOOKSPACE(Gothic_II_Addon, GetGameVersion() == ENGINE);
#include "Plugin.hpp"
#endif

#undef GOTHIC_NAMESPACE
#undef ENGNE
HOOKSPACE(Global, true);

NH::Bass::BassOptions* NH::Bass::Options = new NH::Bass::BassOptions{};

EXTERN_C_START
__declspec(dllexport) void Game_Entry()
{
	NH::Bass::Options->LoggerLevelUnion = "DEBUG";
	NH::Bass::Options->LoggerLevelZSpy = "DEBUG";
}

__declspec(dllexport) void Game_Loop() 
{
	auto* engine = NH::Bass::Engine::Initialize();
	engine->GetEM().Update();
}

__declspec(dllexport) void Game_DefineExternals() 
{
	if (GetGameVersion() == Engine_G1)
	{
		Gothic_I_Classic::DefineExternals();
	}
	if (GetGameVersion() == Engine_G2A)
	{
		Gothic_II_Addon::DefineExternals();
	}
}
EXTERN_C_END