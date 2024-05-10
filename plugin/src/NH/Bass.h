#pragma once

#include <NH/BassTypes.h>
#include <NH/BassEventManager.h>
#include <NH/BassChannel.h>
#include <NH/Logger.h>
#include <vector>
#include <mutex>
#include <chrono>

namespace NH
{
	namespace Bass
	{
		class Engine
		{
            static NH::Logger* log;
			static Engine* s_Instance;
			bool m_Initialized;
			float m_MasterVolume = 1.0f;
			Union::Array<MusicFile> m_MusicFiles{};
			std::vector<Channel> m_Channels;
			Channel* m_ActiveChannel = nullptr;
			EventManager m_EventManager{};

			std::mutex m_PlayMusicMutex;
			std::vector<MusicDefRetry> m_PlayMusicRetryList;

		public:
			static Engine* GetInstance();

			MusicFile& CreateMusicBuffer(const Union::StringUTF8& filename);

			void PlayMusic(const MusicDef& musicDef);

			void Update();

			void SetVolume(float volume);

			float GetVolume() const;

			EventManager& GetEM();

			void StopMusic();

		private:
			Engine();

			MusicFile* GetMusicFile(const Union::StringUTF8& filename);

			Channel* FindAvailableChannel();

		public:
			static Union::StringUTF8 ErrorCodeToString(int code);
		};
	}
}