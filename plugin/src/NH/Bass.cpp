#include "NH/BassOptions.h"
#include "NH/Bass.h"
#include "NH/Union.h"
#include <algorithm>

namespace NH
{
	namespace Bass
	{
		Engine* Engine::s_Instance = nullptr;

		Engine* Engine::GetInstance()
		{
			if (!s_Instance)
			{
				s_Instance = new Engine();
			}
			return s_Instance;
		}

		MusicFile& Engine::CreateMusicBuffer(const Union::StringUTF8& filename)
		{
			for (size_t i = 0; i < m_MusicFiles.GetCount(); i++)
			{
				MusicFile& m = m_MusicFiles[i];
				if (m.Filename == filename) {
					Log::Info("BassEngine", Union::StringUTF8("CreateMusicBuffer: Buffer already exists for ") + filename);
					return m;
				}
			}

			Log::Info("BassEngine", Union::StringUTF8("CreateMusicBuffer: New buffer for ") + filename);
		
			uint32_t index = m_MusicFiles.Insert({ filename, std::vector<char>() });
			return m_MusicFiles[index];
		}

		void Engine::PlayMusic(const MusicDef& musicDef)
		{
			std::lock_guard<std::mutex> guard(m_PlayMusicMutex);

			if (!m_Initialized)
			{
				return;
			}

			const MusicFile* file = GetMusicFile(musicDef.Filename);

			if (!file)
			{
				Log::Error("BassEngine", Union::StringUTF8("Can not play ") + musicDef.Filename + ". Music file is not loaded.");
				return;
			}

			if (!file->Ready && file->Loading)
			{
				Log::Info("BassEngine", musicDef.Filename + " is loading, will PlayMusic retry in 500ms");
				MusicDefRetry retry{ MusicDef(musicDef), 500 };
				m_PlayMusicRetryList.emplace_back(retry);
				return;
			}

			if (!file->Ready)
			{
				Log::Error("BassEngine", Union::StringUTF8("Invalid sate: ") + musicDef.Filename + ". Music not ready and not loading.");
				return;
			}

			Channel* channel = FindAvailableChannel();
			if (channel == nullptr)
			{
				Log::Error("BassEngine", Union::StringUTF8("Can not play. No channel is available."));
				return;
			}

			Log::Info("BassEngine", Union::StringUTF8("Starting playback: ") + musicDef.Filename);
			if (m_ActiveChannel)
			{
				m_ActiveChannel->Stop();
			}

			m_ActiveChannel = channel;
			m_ActiveChannel->Play(musicDef, file);
		}

		void Engine::Update()
		{
			if (!m_Initialized)
			{
				return;
			}

			static auto lastTimestamp = std::chrono::system_clock::now();
			auto now = std::chrono::system_clock::now();
			uint64_t delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimestamp).count();
			lastTimestamp = now;

			for(auto& retry : m_PlayMusicRetryList)
			{
				retry.delayMs -= delta;
				if (retry.delayMs < 0)
				{
					Log::Debug("BassEngine", Union::StringUTF8("PlayMusic retry: ") + retry.musicDef.Filename);
					PlayMusic(retry.musicDef);
				}
			}
			std::erase_if(m_PlayMusicRetryList, [](const MusicDefRetry& retry) { return retry.delayMs < 0; });

			BASS_Update(delta);

			GetEM().Update();
		}

		void Engine::SetVolume(float volume)
		{
			if (!m_Initialized)
			{
				return;
			}

			if (volume > 1.0f)
			{
				Log::Warn("BassEngine", Union::StringUTF8("SetVolume ") + Union::StringUTF8(volume) + " clamped to 1.0f");
				volume = 1.0f;
			}
			if (volume < 0.0f)
			{
				Log::Warn("BassEngine", Union::StringUTF8("SetVolume ") + Union::StringUTF8(volume) + " clamped to 0.0f");
				volume = 0.0f;
			}

			m_MasterVolume = volume;
			BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000 * m_MasterVolume);
		}

		float Engine::GetVolume() const
		{
			return m_MasterVolume;
		}

		EventManager& Engine::GetEM()
		{
			return m_EventManager;
		}

		void Engine::StopMusic()
		{
			if (!m_Initialized)
			{
				return;
			}

			for (auto& channel : m_Channels)
			{
				channel.Stop();
			}

			m_ActiveChannel = nullptr;
		}

		Engine::Engine()
		{
			size_t deviceIndex = 0;
			BASS_DEVICEINFO deviceInfo;
			for (size_t i = 1; BASS_GetDeviceInfo(i, &deviceInfo); i++)
			{
				bool enabled = deviceInfo.flags & BASS_DEVICE_ENABLED;
				bool isDefault = deviceInfo.flags & BASS_DEVICE_DEFAULT;

				Log::Info("BassEngine", Union::StringUTF8("Available device: ") + deviceInfo.name
					+ Union::StringUTF8(", driver: ") + deviceInfo.driver
					+ Union::StringUTF8(", enabled: ") + Union::StringUTF8(enabled ? "true" : "false")
					+ Union::StringUTF8(", default: ") + Union::StringUTF8(isDefault ? "true" : "false"));

				if (enabled && isDefault)
				{
					deviceIndex = i;
					break;
				}
			}

			BASS_GetDeviceInfo(deviceIndex, &deviceInfo);
			Log::Info("BassEngine", Union::StringUTF8("Selected device: ") + deviceInfo.name);

			m_Initialized = BASS_Init(deviceIndex, 44100, 0, nullptr, nullptr);
			if (!m_Initialized)
			{
				Log::Error("BassEngine", Union::StringUTF8("Could not initialize BASS: ") + ErrorCodeToString(BASS_ErrorGetCode()));
				return;
			}

			BASS_INFO info;
			BASS_GetInfo(&info);
			Log::Info("BassEngine", Union::StringUTF8("BASS Sample Rate: ") + Union::StringUTF8(info.freq) + Union::StringUTF8(" Hz"));

			static constexpr size_t Channels_Max = 8;
			m_Channels.resize(Channels_Max, Channel{m_EventManager});
		}

		MusicFile* Engine::GetMusicFile(const Union::StringUTF8& filename)
		{
			for (int i = 0; i < m_MusicFiles.GetCount(); i++)
			{
				if (m_MusicFiles[i].Filename == filename)
				{
					return &m_MusicFiles[i];
				}
			}
			return nullptr;
		}

		Channel* Engine::FindAvailableChannel()
		{
			for (int i = 0; i < m_Channels.size(); i++)
			{
				if (m_Channels[i].IsAvailable())
				{
					return &m_Channels[i];
				}
			}
			return nullptr;
		}

		Union::StringUTF8 Engine::ErrorCodeToString(const int code)
		{
			switch (code)
			{
			case 0:
				return Union::StringUTF8("BASS_OK");
			case 1:
				return Union::StringUTF8("BASS_ERROR_MEM");
			case 2:
				return Union::StringUTF8("BASS_ERROR_FILEOPEN");
			case 3:
				return Union::StringUTF8("BASS_ERROR_DRIVER");
			case 4:
				return Union::StringUTF8("BASS_ERROR_BUFLOST");
			case 5:
				return Union::StringUTF8("BASS_ERROR_HANDLE");
			case 6:
				return Union::StringUTF8("BASS_ERROR_FORMAT");
			case 7:
				return Union::StringUTF8("BASS_ERROR_POSITION");
			case 8:
				return Union::StringUTF8("BASS_ERROR_INIT");
			case 9:
				return Union::StringUTF8("BASS_ERROR_START");
			case 10:
				return Union::StringUTF8("BASS_ERROR_SSL");
			case 11:
				return Union::StringUTF8("BASS_ERROR_REINIT");
			case 14:
				return Union::StringUTF8("BASS_ERROR_ALREADY");
			case 17:
				return Union::StringUTF8("BASS_ERROR_NOTAUDIO");
			case 18:
				return Union::StringUTF8("BASS_ERROR_NOCHAN");
			case 19:
				return Union::StringUTF8("BASS_ERROR_ILLTYPE");
			case 20:
				return Union::StringUTF8("BASS_ERROR_ILLPARAM");
			case 21:
				return Union::StringUTF8("BASS_ERROR_NO3D");
			case 22:
				return Union::StringUTF8("BASS_ERROR_NOEAX");
			case 23:
				return Union::StringUTF8("BASS_ERROR_DEVICE");
			case 24:
				return Union::StringUTF8("BASS_ERROR_NOPLAY");
			case 25:
				return Union::StringUTF8("BASS_ERROR_FREQ");
			case 27:
				return Union::StringUTF8("BASS_ERROR_NOTFILE");
			case 29:
				return Union::StringUTF8("BASS_ERROR_NOHW");
			case 31:
				return Union::StringUTF8("BASS_ERROR_EMPTY");
			case 32:
				return Union::StringUTF8("BASS_ERROR_NONET");
			case 33:
				return Union::StringUTF8("BASS_ERROR_CREATE");
			case 34:
				return Union::StringUTF8("BASS_ERROR_NOFX");
			case 37:
				return Union::StringUTF8("BASS_ERROR_NOTAVAIL");
			case 38:
				return Union::StringUTF8("BASS_ERROR_DECODE");
			case 39:
				return Union::StringUTF8("BASS_ERROR_DX");
			case 40:
				return Union::StringUTF8("BASS_ERROR_TIMEOUT");
			case 41:
				return Union::StringUTF8("BASS_ERROR_FILEFORM");
			case 42:
				return Union::StringUTF8("BASS_ERROR_SPEAKER");
			case 43:
				return Union::StringUTF8("BASS_ERROR_VERSION");
			case 44:
				return Union::StringUTF8("BASS_ERROR_CODEC");
			case 45:
				return Union::StringUTF8("BASS_ERROR_ENDED");
			case 46:
				return Union::StringUTF8("BASS_ERROR_BUSY");
			case 47:
				return Union::StringUTF8("BASS_ERROR_UNSTREAMABLE");
			case 48:
				return Union::StringUTF8("BASS_ERROR_PROTOCOL");
			case 49:
				return Union::StringUTF8("BASS_ERROR_DENIED");
			default:
				return Union::StringUTF8("BASS_ERROR_UNKNOWN");
			}
		}
	}
}