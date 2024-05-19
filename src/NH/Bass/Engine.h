#pragma once

#include "CommonTypes.h"
#include "EventManager.h"
#include "Channel.h"
#include "TransitionScheduler.h"
#include "NH/Logger.h"
#include "NH/HashString.h"
#include <vector>
#include <mutex>
#include <chrono>
#include <unordered_map>

namespace NH::Bass
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
		TransitionScheduler m_TransitionScheduler{};
		std::unordered_map<HashString, MusicDef> m_MusicDefs;

		std::mutex m_PlayMusicMutex;
		std::vector<MusicDefRetry> m_PlayMusicRetryList;

	public:
		static Engine* GetInstance();

		MusicFile& CreateMusicBuffer(const Union::StringUTF8& filename);

		void PlayMusic(MusicDef musicDef);

		void Update();

		void SetVolume(float volume);

		float GetVolume() const;

		EventManager& GetEM();

		TransitionScheduler& GetTransitionScheduler();

		void StopMusic();

	private:
		Engine();

		MusicFile* GetMusicFile(const Union::StringUTF8& filename);

		Channel* FindAvailableChannel();

		void FinalizeScheduledMusic(const MusicDef& musicDef);

	public:
		static Union::StringUTF8 ErrorCodeToString(int code);
	};
}
