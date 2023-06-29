#pragma once

namespace NH
{
	namespace Bass
	{
		struct BassOptions
		{
			float TransitionTime = 2000.0f;
			bool ForceDisableReverb = false;
			bool ForceFadeTransition = false;
		};

		extern BassOptions Options;
	}
}