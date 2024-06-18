// Headers required for GOTHIC_NAMESPACE files. Don't delete.
#include "BuildInfo.h"
#include "NH/Bass/Options.h"
#include "NH/Bass/Engine.h"
#include "NH/Bass/Command.h"
#include "NH/Bass/EngineCommands.h"

#pragma warning(push, 1)
#include <Union/Hook.h>
#include <ZenGin/zGothicAPI.h>
#pragma warning(pop)

#ifdef __G1
#define GOTHIC_NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
HOOKSPACE(Gothic_I_Classic, GetGameVersion() == ENGINE);

#include "Plugin.hpp"

#endif

#ifdef __G1A
#define GOTHIC_NAMESPACE Gothic_I_Addon
#define ENGINE Engine_G1A
HOOKSPACE(Gothic_I_Addon, GetGameVersion() == ENGINE);

#include "Plugin.hpp"

#endif

#ifdef __G2
#define GOTHIC_NAMESPACE Gothic_II_Classic
#define ENGINE Engine_G2
HOOKSPACE(Gothic_II_Classic, GetGameVersion() == ENGINE);

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