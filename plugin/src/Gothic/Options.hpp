#include <NH/BassOptions.h>

namespace GOTHIC_NAMESPACE
{
	void ApplyOptions()
	{
		NH::Bass::Options.TransitionTime = zoptions->ReadReal("BASSMUSIC", "TransitionTime", 2000.0f);
		NH::Bass::Options.ForceDisableReverb = zoptions->ReadBool("BASSMUSIC", "ForceDisableReverb", false);
		NH::Bass::Options.ForceFadeTransition = zoptions->ReadBool("BASSMUSIC", "ForceFadeTransition", false);
		NH::Bass::Options.CreateMainParserCMusicTheme = zoptions->ReadBool("BASSMUSIC", "CreateMainParserCMusicTheme", true);
	}
}