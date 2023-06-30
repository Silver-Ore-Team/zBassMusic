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

			Log::Info("BassEngine", Union::StringUTF8("Starting playback: ") + musicDef.Filename);

			if (m_FrontChannel.Stream > 0)
			{
				Log::Debug("BassEngine", Union::StringUTF8("Stopping front channel: ") + m_FrontChannel.Music.Filename);

				if (m_BackChannel.Stream > 0)
				{
					BASS_ChannelFree(m_BackChannel.Stream);
				}

				m_BackChannel = m_FrontChannel;
				m_FrontChannel = {};
				if (m_BackChannel.Playing) {
					if (m_BackChannel.Music.EndTransition.Type == TransitionType::FADE)
					{
						BASS_ChannelSlideAttribute(m_BackChannel.Stream, BASS_ATTRIB_VOL, 0.0f, m_BackChannel.Music.EndTransition.Duration);
						BASS_ChannelSetSync(m_BackChannel.Stream, BASS_SYNC_SLIDE, 0, SyncSlide, this);
					}
					else
					{
						BASS_ChannelFree(m_BackChannel.Stream);
					}
					m_BackChannel.Playing = false;
				}
				else
				{
					BASS_ChannelFree(m_BackChannel.Stream);
				}
			}

			HSTREAM stream = BASS_StreamCreateFile(true, file->Buffer.data(), 0, file->Buffer.size(), 0);
			if (!stream)
			{
				Log::Error("BassEngine", Union::StringUTF8("Could not create stream: ") + musicDef.Filename);
				Log::Error("BassEngine", Union::StringUTF8("Could not create stream: ") + ErrorCodeToString(BASS_ErrorGetCode()));
				return;
			}

			Log::Debug("BassEngine", Union::StringUTF8("Stream created: ") + musicDef.Filename);

			m_FrontChannel = {stream, true, musicDef};
			BASS_ChannelStart(m_FrontChannel.Stream);

			Log::Debug("BassEngine", Union::StringUTF8("Channel started: ") + musicDef.Filename);

			if (Options->ForceFadeTransition)
			{
				Log::Info("BassEngine", Union::StringUTF8("BASSENGINE.ForceFadeTransition is set, forcing TransitionType::FADE"));
				m_FrontChannel.Music.StartTransition.Type = TransitionType::FADE;
				m_FrontChannel.Music.EndTransition.Type = TransitionType::FADE;
			}

			if (m_FrontChannel.Music.StartTransition.Type == TransitionType::FADE)
			{
				BASS_ChannelSetAttribute(m_FrontChannel.Stream, BASS_ATTRIB_VOL, 0.0f);
				BASS_ChannelSlideAttribute(m_FrontChannel.Stream, BASS_ATTRIB_VOL, m_FrontChannel.Music.Volume,
					m_FrontChannel.Music.StartTransition.Duration);
			}
			else
			{
				BASS_ChannelSetAttribute(m_FrontChannel.Stream, BASS_ATTRIB_VOL, m_FrontChannel.Music.Volume);
			}

			if (m_FrontChannel.Music.Loop)
			{
				BASS_ChannelFlags(m_FrontChannel.Stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
				Log::Debug("BassEngine", Union::StringUTF8("Loop set: ") + musicDef.Filename);
			}

			if (!Options->ForceDisableReverb && m_FrontChannel.Music.Effects.Reverb)
			{
				HFX fx = BASS_ChannelSetFX(m_FrontChannel.Stream, BASS_FX_DX8_REVERB, 1);
				BASS_DX8_REVERB params{0, m_FrontChannel.Music.Effects.ReverbMix, m_FrontChannel.Music.Effects.ReverbTime, 0.001f};
				if (!BASS_FXSetParameters(fx, (void*)&params))
				{
					Log::Error("BassEngine", Union::StringUTF8("Could not set reverb FX: ") + musicDef.Filename);
					Log::Error("BassEngine", Union::StringUTF8("Could not set reverb FX: ") + ErrorCodeToString(BASS_ErrorGetCode()));
				}
				Log::Debug("BassEngine", Union::StringUTF8("Reverb set: ") + musicDef.Filename);
			}

			if (m_FrontChannel.Music.EndTransition.Type != TransitionType::NONE)
			{
				const QWORD length = BASS_ChannelGetLength(m_FrontChannel.Stream, BASS_POS_BYTE);
				const QWORD transitionBytes = BASS_ChannelSeconds2Bytes(m_FrontChannel.Stream, m_FrontChannel.Music.EndTransition.Duration / 1000.0f);
				const QWORD offset = length - transitionBytes;
				BASS_ChannelSetSync(m_FrontChannel.Stream, BASS_SYNC_POS, offset, SyncTransitionPos, this);
				Log::Debug("BassEngine", Union::StringUTF8("SyncTransitionPos set: ") + musicDef.Filename);
			}

			BASS_ChannelSetSync(m_FrontChannel.Stream, BASS_SYNC_END, 0, SyncEnd, this);
			Log::Debug("BassEngine", Union::StringUTF8("SyncEnd set: ") + musicDef.Filename);

			m_EventManager.DispatchEvent(EventType::MUSIC_CHANGE, &m_FrontChannel.Music);
		}

		void Engine::Update(const unsigned long time)
		{
			if (!m_Initialized)
			{
				return;
			}

			BASS_Update(time);

			if (m_FrontChannel.Stream > 0 && m_FrontChannel.Playing)
			{
				m_EventManager.DispatchEvent(EventType::MUSIC_ACTIVE, &m_FrontChannel.Music);
			}
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

			if (m_FrontChannel.Stream > 0 && m_FrontChannel.Playing)
			{
				Log::Debug("BassEngine", Union::StringUTF8("Stopping front channel: ") + m_FrontChannel.Music.Filename);

				if (m_FrontChannel.Music.StartTransition.Type == TransitionType::FADE)
				{
					BASS_ChannelSlideAttribute(m_FrontChannel.Stream, BASS_ATTRIB_VOL, 0.0f, m_FrontChannel.Music.StartTransition.Duration);
					BASS_ChannelSetSync(m_BackChannel.Stream, BASS_SYNC_SLIDE, 0, SyncSlide, this);
				}
				else
				{
					BASS_ChannelFree(m_FrontChannel.Stream);
				}
			}

			if (m_BackChannel.Stream > 0 && m_BackChannel.Playing)
			{
				Log::Debug("BassEngine", Union::StringUTF8("Stopping back channel: ") + m_BackChannel.Music.Filename);

				if (m_BackChannel.Music.StartTransition.Type == TransitionType::FADE)
				{
					BASS_ChannelSlideAttribute(m_BackChannel.Stream, BASS_ATTRIB_VOL, 0.0f, m_BackChannel.Music.StartTransition.Duration);
					BASS_ChannelSetSync(m_BackChannel.Stream, BASS_SYNC_SLIDE, 0, SyncSlide, this);

				}
				else
				{
					BASS_ChannelFree(m_BackChannel.Stream);
				}
			}
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

		void Engine::SyncTransitionPos(HSYNC, DWORD channel, DWORD data, void* _this)
		{
			const auto self = static_cast<Engine*>(_this);
			if (self->m_FrontChannel.Stream == channel)
			{
				self->GetEM().DispatchEvent(EventType::MUSIC_TRANSITION, &self->m_FrontChannel.Music, self->m_FrontChannel.Music.EndTransition.Duration);
			}
		}

		void Engine::SyncEnd(HSYNC, DWORD channel, DWORD data, void* _this)
		{
			const auto self = static_cast<Engine*>(_this);
			if (self->m_FrontChannel.Stream == channel)
			{
				if (!self->m_FrontChannel.Music.Loop)
				{
					self->m_FrontChannel.Playing = false;
				}
				self->GetEM().DispatchEvent(EventType::MUSIC_END, &self->m_FrontChannel.Music);
			}
		}

		void Engine::SyncSlide(HSYNC, DWORD channel, DWORD data, void* _this)
		{
			const auto self = static_cast<Engine*>(_this);
			if (data == BASS_ATTRIB_VOL)
			{
				float volume;
				BASS_ChannelGetAttribute(channel, BASS_ATTRIB_VOL, &volume);
				if (volume < 0.0001f)
				{
					if (self->m_FrontChannel.Stream == channel && !self->m_FrontChannel.Playing)
					{
						BASS_ChannelFree(self->m_FrontChannel.Stream);
						self->m_FrontChannel.Stream = 0;
					}
					else if (self->m_BackChannel.Stream == channel && !self->m_BackChannel.Playing)
					{
						BASS_ChannelFree(self->m_BackChannel.Stream);
						self->m_BackChannel.Stream = 0;
					}
					else
					{
						BASS_ChannelFree(channel);
					}
				}
			}
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