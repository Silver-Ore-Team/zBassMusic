#pragma once

#include <bass.h>
#include <Union/String.h>

namespace NH
{
	namespace Bass
	{
		enum class TransitionType
		{
			NONE,
			FADE
		};

		struct Transition
		{
			TransitionType Type = TransitionType::NONE;
			unsigned int Duration = 0;
		};

		struct MusicFile
		{
			Union::StringUTF8 Filename;
			std::vector<char> Buffer;
			bool Ready = false;
			bool Loading = false;
		};

		struct MusicDef
		{
			Union::StringUTF8 Filename;
			Union::StringUTF8 Name = Filename;
			float Volume = 1.0f;
			bool Loop = false;
			Transition StartTransition{};
			Transition EndTransition{};
			struct AudioEffects
			{
				bool Reverb = false;
				float ReverbMix = 0.0f;
				float ReverbTime = 1000.0f;
			} Effects{};
		};

		struct MusicDefRetry
		{
			MusicDef musicDef;
			int32_t delayMs;
		};

		struct MusicChannel
		{
			HSTREAM Stream = 0;
			bool Playing = false;
			MusicDef Music{};
		};
	}
}