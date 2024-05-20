#include "Options.h"
#include "Engine.h"
#include <algorithm>

namespace NH::Bass
{
	NH::Logger* Engine::log = NH::CreateLogger("zBassMusic::Engine");

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
		for (auto& [key, m]: m_MusicFiles)
		{
			if (key == filename)
			{
				log->Debug("Buffer already exists for {0}", filename);
				return m;
			}
		}

		m_MusicFiles.emplace(std::make_pair(filename, MusicFile{ filename, std::vector<char>() }));
		log->Debug("New buffer for {0}", filename);

		return m_MusicFiles.at(filename);
	}

	void Engine::PlayMusic(MusicDef inMusicDef)
	{
		std::lock_guard<std::mutex> guard(m_PlayMusicMutex);
		if (!m_Initialized)
		{
			return;
		}

		// We are taking ownership of the MusicDef from here to gurarantee it's lifetime.
		// MusicDef lives as long as the Engine instance and other classes expect it to stay
		// at the same place in memory, so if we receive a second MusicDef with the same filename,
		// we just copy its' data to our instance.
		auto musicDefKey = HashString(inMusicDef.Filename);
		if (m_MusicDefs.find(musicDefKey) == m_MusicDefs.end())
		{
			m_MusicDefs[musicDefKey] = std::move(inMusicDef);
		}
		else
		{
			m_MusicDefs[musicDefKey].CopyFrom(inMusicDef);
		}

		const MusicFile* file = GetMusicFile(m_MusicDefs[musicDefKey].Filename);

		if (!file)
		{
			log->Error("Could not play {0}. Music file is not loaded.\n  at {1}:{2}",
					m_MusicDefs[musicDefKey].Filename, __FILE__, __LINE__);
			return;
		}

		if (!file->Ready && file->Loading)
		{
			static int32_t delay = 10;
			log->Debug("{0} is loading, will retry after {1} ms", m_MusicDefs[musicDefKey].Filename, delay);
			MusicDefRetry retry{ MusicDef(m_MusicDefs[musicDefKey]), delay };
			m_PlayMusicRetryList.emplace_back(retry);
			return;
		}

		if (!file->Ready)
		{
			log->Error("Invalid state. MusicDef is not ready but not loading {0}\n  at {1}:{2}",
					m_MusicDefs[musicDefKey].Filename, __FILE__, __LINE__);
			return;
		}

		if (!m_ActiveChannel)
		{
			FinalizeScheduledMusic(m_MusicDefs[musicDefKey]);
			return;
		}

		m_TransitionScheduler.Schedule(*m_ActiveChannel, m_MusicDefs[musicDefKey]);
	}

	void Engine::FinalizeScheduledMusic(const NH::Bass::MusicDef& musicDef)
	{
		Channel* channel = FindAvailableChannel();
		if (channel == nullptr)
		{
			log->Error("Could not play {0}. No channel is available.\n  at {1}:{2}",
					musicDef.Filename, __FILE__, __LINE__);
			return;
		}

		log->Info("Starting playback: {0}", musicDef.Filename);
		if (m_ActiveChannel)
		{
			m_ActiveChannel->Stop();
		}

		m_ActiveChannel = channel;
		m_ActiveChannel->Play(musicDef, GetMusicFile(musicDef.Filename));
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

		for (auto& retry: m_PlayMusicRetryList)
		{
			retry.delayMs -= delta;
			if (retry.delayMs < 0)
			{
				log->Trace("PlayMusic({0})", retry.musicDef.Filename);
				PlayMusic(retry.musicDef);
			}
		}
		std::erase_if(m_PlayMusicRetryList, [](const MusicDefRetry& retry)
		{ return retry.delayMs < 0; });

		m_TransitionScheduler.Update([this](const MusicDef& musicDef)
		{
			log->Trace("onReady from scheduler {0}", musicDef.Filename);
			FinalizeScheduledMusic(musicDef);
		});

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
			log->Warning("SetVolume({0}f) clamped to 1.0f", volume);
			volume = 1.0f;
		}
		if (volume < 0.0f)
		{
			log->Warning("SetVolume({0}f) clamped to 0.0f", volume);
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

	TransitionScheduler& Engine::GetTransitionScheduler()
	{
		return m_TransitionScheduler;
	}

	void Engine::StopMusic()
	{
		if (!m_Initialized)
		{
			return;
		}

		for (auto& channel: m_Channels)
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

			if (enabled && isDefault)
			{
				deviceIndex = i;
				break;
			}
		};

		m_Initialized = BASS_Init(deviceIndex, 44100, 0, nullptr, nullptr);
		if (!m_Initialized)
		{
			log->Error("Could not initialize BASS using BASS_Init\n  {0}\n  at {1}:{2}",
					ErrorCodeToString(BASS_ErrorGetCode()), __FILE__, __LINE__);
			return;
		}

		BASS_INFO info;
		BASS_GetInfo(&info);
		log->Trace("Sample Rate: {0} Hz", info.freq);

		static constexpr size_t Channels_Max = 8;
		m_Channels.clear();
		for (size_t i = 0; i < Channels_Max; i++)
		{
			m_Channels.emplace_back(i, m_EventManager);
		}

		log->Info("Initialized with device: {0}", deviceIndex);
	}

	MusicFile* Engine::GetMusicFile(const Union::StringUTF8& filename)
	{
		for (auto& [key, m]: m_MusicFiles)
		{
			if (key == filename)
			{
				return &m;
			}
		}

		return nullptr;
	}

	Channel* Engine::FindAvailableChannel()
	{
		for (auto& channel: m_Channels)
		{
			if (channel.IsAvailable())
			{
				return &channel;
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