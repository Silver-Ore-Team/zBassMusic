// Disable macro redefinition warning 
#pragma warning(disable: 4005)

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

HOOKSPACE(Global, true);

EXTERN_C_START
__declspec(dllexport) void Game_ApplyOptions() {
	switch (GetGameVersion())
    {
    case Engine_G1:
        Gothic_I_Classic::ApplyOptions();
        break;
    case Engine_G2A:
        Gothic_II_Addon::ApplyOptions();
        break;
    }
}

__declspec(dllexport) void Game_DefineExternals() {
    switch (GetGameVersion())
    {
    case Engine_G1:
        Gothic_I_Classic::DefineExternals();
        break;
    case Engine_G2A:
        Gothic_II_Addon::DefineExternals();
        break;
    }
}
EXTERN_C_END
