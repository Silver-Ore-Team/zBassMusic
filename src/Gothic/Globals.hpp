#pragma warning(push, 1)
#include <deque>
#pragma warning(pop)

namespace GOTHIC_NAMESPACE
{
    struct GlobalsDef
    {
        zSTRING CMusicThemeClass = "C_MUSICTHEME";
        zSTRING BassMusicThemeClassName = "C_BassMusic_Theme";
        zSTRING BassMusicThemeAudioClassName = "C_BassMusic_ThemeAudio";
        zSTRING BassMusic_ActiveThemeFilename = "";
        zSTRING BassMusic_ActiveThemeID = "";
        zSTRING BassMusic_EventThemeFilename = "";
        zSTRING BassMusic_EventThemeID = "";
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
