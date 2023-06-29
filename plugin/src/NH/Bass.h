#pragma once

#include <NH/BassTypes.h>
#include <NH/BassEventManager.h>

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
			MusicChannel m_FrontChannel{};
			MusicChannel m_BackChannel{};
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

			static void CALLBACK SyncTransitionPos(HSYNC, DWORD channel, DWORD data, void* _this);

			static void CALLBACK SyncEnd(HSYNC, DWORD channel, DWORD data, void* _this);

			static void CALLBACK SyncSlide(HSYNC, DWORD channel, DWORD data, void* _this);

			static Union::StringUTF8 ErrorCodeToString(int code);
		};
	}
}