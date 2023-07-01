#include "NH/BassOptions.h"
#include "NH/Bass.h"
#include "NH/Union.h"

namespace NH
{
	namespace Bass
	{
		Engine* Engine::s_Instance = nullptr;

		Engine* Engine::Initialize()
		{
			if (!s_Instance)
			{
				s_Instance = new Engine();
			}
			return s_Instance;
		}

		void Engine::LoadMusicFile(const Union::StringUTF8& filename, const std::vector<char>& buffer)
		{
			for (auto m : m_MusicFiles)
			{
				if (m.Filename == filename) {
					Log::Info("BassEngine", Union::StringUTF8("LoadMusicFile: Buffer already exists for ") + filename);
					return;
				}
			}
			Log::Info("BassEngine", Union::StringUTF8("LoadMusicFile: New buffer for ") + filename);
			Log::Info("BassEngine", Union::StringUTF8("LoadMusicFile: New buffer length: ") + Union::StringUTF8(buffer.size()));

			m_MusicFiles.Insert({filename, buffer});

			size_t memoryUsage = 0;
			for (auto m : m_MusicFiles)
			{
				memoryUsage = m.Buffer.size();
			}
			Log::Debug("BassEngine", Union::StringUTF8("m_MusicFiles loaded files: ") + Union::StringUTF8(m_MusicFiles.GetCount()));
			Log::Debug("BassEngine", Union::StringUTF8("m_MusicFiles memory usage: ") + Union::StringUTF8(static_cast<float>(memoryUsage) / 1024 / 1024) + Union::StringUTF8(" MB"));
		}

		void Engine::PlayMusic(const MusicDef& musicDef)
		{
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

			m_EventManager.DispatchEvent(EventType::MUSIC_CHANGE, m_FrontChannel.Music);
		}

		void Engine::Update(const unsigned long time)
		{
			if (!m_Initialized)
			{
				return;
			}

			BASS_Update(time);
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
			m_Initialized = BASS_Init(-1, 44100, 0, nullptr, nullptr);
			if (!m_Initialized)
			{
				Log::Error("BassEngine", Union::StringUTF8("Could not initialize BASS: ") + ErrorCodeToString(BASS_ErrorGetCode()));
				return;
			}
			BASS_DEVICEINFO device{};
			BASS_GetDeviceInfo(BASS_GetDevice(), &device);
			Log::Info("BassEngine", Union::StringUTF8("BASS Device: ") + device.name);
			Log::Info("BassEngine", Union::StringUTF8("BASS Driver: ") + device.driver);
			Log::Info("BassEngine", Union::StringUTF8("BASS Sample Rate: 44100 Hz"));

			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
			m_Channels.emplace_back(Channel(m_EventManager));
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