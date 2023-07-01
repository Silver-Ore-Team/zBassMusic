#pragma once

#include <NH/BassTypes.h>
#include <NH/BassEventManager.h>

namespace NH
{
	namespace Bass
	{
		enum class ChannelStatus
		{
			AVAILABLE,
			PLAYING,
			FADING_OUT
		};

		class Channel
		{
			EventManager& m_EventManager;
			ChannelStatus m_Status = ChannelStatus::AVAILABLE;
			HSTREAM m_Stream = 0;
			MusicDef m_Music{};

		public:
			explicit Channel(EventManager& em) : m_EventManager(em) {};

			void Play(const MusicDef& music, const MusicFile* file);

			void Stop();

			bool IsAvailable() { return m_Status == ChannelStatus::AVAILABLE; };

			static void CALLBACK OnTransitionSync(HSYNC, DWORD channel, DWORD data, void* userData);

			static void CALLBACK OnEndSync(HSYNC, DWORD channel, DWORD data, void* userData);

			static void CALLBACK OnVolumeSlideSync(HSYNC, DWORD channel, DWORD data, void* userData);
		};
	}
}