#pragma once

#include <NH/BassTypes.h>
#include <NH/BassEventManager.h>
#include <NH/BassChannel.h>
#include <vector>

namespace NH
{
	namespace Bass
	{
		class Engine
		{
			static Engine* s_Instance;
			bool m_Initialized;
			float m_MasterVolume = 1.0f;
			Union::Array<MusicFile> m_MusicFiles{};
			std::vector<Channel> m_Channels;
			Channel* m_ActiveChannel = nullptr;
			EventManager m_EventManager{};


		public:
			static Engine* Initialize();

			void LoadMusicFile(const Union::StringUTF8& filename, const std::vector<char>& buffer);

			void PlayMusic(const MusicDef& musicDef);

			void Update(unsigned long time);

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