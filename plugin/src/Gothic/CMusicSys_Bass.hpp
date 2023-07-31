namespace GOTHIC_NAMESPACE
{
	namespace BassEvent
	{
		void Event_OnEnd(const NH::Bass::MusicDef musicDef, int data, void* userData)
		{
			NH::Log::Debug("Event", Union::StringUTF8("Event_OnEnd: ") + Union::StringUTF8(musicDef.Filename.ToChar()));

			for (int i = 0; i < Globals->Event_OnEnd_Functions.GetNumInList(); i++)
			{
				const int funcId = Globals->Event_OnEnd_Functions[i];
				Globals->BassMusic_EventThemeFilename = zSTRING(musicDef.Filename.ToChar());
				Globals->BassMusic_EventThemeID = zSTRING(musicDef.Name.ToChar());
				parser->CallFunc(funcId);
			}
		}

		void Event_OnTransition(const NH::Bass::MusicDef musicDef, int data, void* userData)
		{
			NH::Log::Debug("Event", Union::StringUTF8("Event_OnTransition: ") + Union::StringUTF8(musicDef.Filename.ToChar())
				+ Union::StringUTF8::Format(", %i ms", data));

			for (int i = 0; i < Globals->Event_OnTransition_Functions.GetNumInList(); i++)
			{
				const int funcId = Globals->Event_OnTransition_Functions[i];
				Globals->BassMusic_EventThemeFilename = zSTRING(musicDef.Filename.ToChar());
				Globals->BassMusic_EventThemeID = zSTRING(musicDef.Name.ToChar());
				parser->CallFunc(funcId, data);
			}
		}

		void Event_OnChange(const NH::Bass::MusicDef musicDef, int data, void* userData)
		{
			NH::Log::Debug("Event", Union::StringUTF8("Event_OnChange: ") + Union::StringUTF8(musicDef.Filename.ToChar()));

			zSTRING filename = zSTRING(musicDef.Filename.ToChar());
			zSTRING name = zSTRING(musicDef.Name.ToChar());

			Globals->BassMusic_ActiveThemeFilename = filename;
			Globals->BassMusic_ActiveThemeID = name;

			for (int i = 0; i < Globals->Event_OnChange_Functions.GetNumInList(); i++)
			{
				const int funcId = Globals->Event_OnChange_Functions[i];
				Globals->BassMusic_EventThemeFilename = zSTRING(musicDef.Filename.ToChar());
				Globals->BassMusic_EventThemeID = zSTRING(musicDef.Name.ToChar());
				parser->CallFunc(funcId);
			}
		}
	}

	class CMusicSys_Bass : public zCMusicSystem
	{
	private:
		NH::Bass::Engine* m_BassEngine;
		zCMusicSys_DirectMusic* m_DirectMusic;
		zCMusicTheme* m_ActiveTheme = nullptr;

		static bool IsDirectMusicFormat(zSTRING const& file)
		{
			return Union::String(file.ToChar()).MakeLower().EndsWith(".sgt");
		}

	public:

		CMusicSys_Bass(NH::Bass::Engine* bassEngine, zCMusicSys_DirectMusic* directMusic)
			: m_BassEngine(bassEngine), m_DirectMusic(directMusic)
		{
			m_BassEngine->GetEM().AddSubscriber(NH::Bass::EventType::MUSIC_END, &BassEvent::Event_OnEnd, this);
			m_BassEngine->GetEM().AddSubscriber(NH::Bass::EventType::MUSIC_TRANSITION, &BassEvent::Event_OnTransition, this);
			m_BassEngine->GetEM().AddSubscriber(NH::Bass::EventType::MUSIC_CHANGE, &BassEvent::Event_OnChange, this);
		}

		zCMusicTheme* LoadThemeByScript(zSTRING const& id) override
		{
			if (s_musicSystemDisabled || !id || id.IsEmpty())
			{
				return nullptr;
			}

			NH::Log::Info("CMusicSys_Bass", Union::StringUTF8::Format("LoadThemeByScript: ") + id.ToChar());

			zSTRING identifier = id;
			if (m_ActiveTheme && identifier.Upper() == m_ActiveTheme->name)
			{
				return m_ActiveTheme;
			}

			zCMusicTheme* theme = zNEW(zCMusicTheme);
			if (NH::Bass::Options->CreateMainParserCMusicTheme && parser->GetSymbol(identifier) != nullptr)
			{
				parser->CreateInstance(identifier, (void*)(&(theme->fileName)));
			}
			else
			{
				parserMusic->CreateInstance(identifier, (void*)(&(theme->fileName)));
			}
			theme->name = identifier;

			if (IsDirectMusicFormat(theme->fileName))
			{
				zDELETE(theme);
				theme = m_DirectMusic->LoadThemeByScript(id);
			}
			else
			{
				zoptions->ChangeDir(DIR_MUSIC);
				zFILE* file = zfactory->CreateZFile(theme->fileName);
				if (file->Exists())
				{
					NH::Bass::MusicFile& musicFileRef = m_BassEngine->CreateMusicBuffer(theme->fileName.ToChar());
					if (!musicFileRef.Ready && !musicFileRef.Loading)
					{
						NH::Log::Info("CMusicSys_Bass", Union::StringUTF8::Format("Loading music: ") + file->GetFullPath().ToChar());

						file->Open(false);
						musicFileRef.Loading = true;
						auto loadingStart = std::chrono::system_clock::now();

						std::thread loadingThread([file, &musicFileRef, loadingStart]() {
							size_t size = file->Size();
							musicFileRef.Buffer.resize(size);
							size_t read = file->Read(musicFileRef.Buffer.data(), size);
							file->Close();

							musicFileRef.Ready = true;
							musicFileRef.Loading = false;

							uint64_t loadingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - loadingStart).count();
								
							NH::Log::Info("CMusicSys_Bass", Union::StringUTF8::Format("%z ready, size ", file->GetFullPath()) + Union::StringUTF8(read));
							NH::Log::Debug("CMusicSys_Bass", Union::StringUTF8::Format("%z loading took %I ms", file->GetFullPath(), loadingTime));
						});

						loadingThread.detach();
					}
				}
				else
				{
					NH::Log::Error("CMusicSys_Bass", Union::StringUTF8::Format("Could not find file: ") + theme->fileName.ToChar());
				}
			}

			return theme;
		}

		void PlayThemeByScript(zSTRING const& id, int manipulate, int* done) override
		{
			if (s_musicSystemDisabled || !id || id.IsEmpty())
			{
				return;
			}

			NH::Log::Info("CMusicSys_Bass", Union::StringUTF8::Format("PlayThemeByScript: ") + id.ToChar());

			if (Globals->FullScriptControl)
			{
				NH::Log::Info("CMusicSys_Bass", Union::StringUTF8::Format("PlayThemeByScript skipped because FullScriptControl is enabled."));
				return;
			}

			zSTRING identifier = id;
			if (m_ActiveTheme && identifier.Upper() == m_ActiveTheme->name)
			{
				return;
			}

			zCMusicTheme* theme = LoadThemeByScript(id);
			if (theme)
			{
				if (IsDirectMusicFormat(theme->fileName))
				{
					m_ActiveTheme = theme;
					m_BassEngine->StopMusic();
					return m_DirectMusic->PlayThemeByScript(id, manipulate, done);
				}

				if (done)
				{
					*done = true;
				}

				switch (manipulate)
				{
				case 1:
					PlayTheme(theme, zMUS_THEME_VOL_DEFAULT, zMUS_TR_END, zMUS_TRSUB_DEFAULT);
					break;
				case 2:
					PlayTheme(theme, zMUS_THEME_VOL_DEFAULT, zMUS_TR_NONE, zMUS_TRSUB_DEFAULT);
					break;
				default:
					PlayTheme(theme);
				}

				return;
			}

			if (done)
			{
				*done = false;
			}
		}

		void PlayTheme(zCMusicTheme* theme, 
			float const& volume = zMUS_THEME_VOL_DEFAULT, 
			zTMus_TransType const& transition = zMUS_TR_DEFAULT, 
			zTMus_TransSubType const& subTransition = zMUS_TRSUB_DEFAULT) override
		{
			NH::Log::Debug(Union::StringUTF8("PlayTheme: ") + theme->fileName.ToChar());

			if (IsDirectMusicFormat(theme->fileName))
			{
				m_BassEngine->StopMusic();
				m_ActiveTheme = theme;
				m_DirectMusic->PlayTheme(theme, volume, transition, subTransition);
				return;
			}

			m_DirectMusic->Stop();

			if (transition != zMUS_TR_DEFAULT)
			{
				theme->trType = transition;
			}

			if (subTransition != zMUS_TRSUB_DEFAULT)
			{
				theme->trSubType = subTransition;
			}

			NH::Bass::MusicDef musicDef{};
			musicDef.Filename = theme->fileName.ToChar();
			musicDef.Name = theme->name.ToChar();
			musicDef.Loop = theme->loop;
			musicDef.Volume = theme->vol;

			if (theme->trType == zMUS_TR_INTRO || theme->trType == zMUS_TR_ENDANDINTRO)
			{
				musicDef.StartTransition.Type = NH::Bass::TransitionType::FADE;
				musicDef.StartTransition.Duration = NH::Bass::Options->TransitionTime;
			}

			if (theme->trType == zMUS_TR_END || theme->trType == zMUS_TR_ENDANDINTRO)
			{
				musicDef.EndTransition.Type = NH::Bass::TransitionType::FADE;
				musicDef.EndTransition.Duration = NH::Bass::Options->TransitionTime;
			}

			if (m_DirectMusic->prefs.globalReverbEnabled)
			{
				musicDef.Effects.Reverb = true;
				musicDef.Effects.ReverbMix = theme->reverbMix;
				musicDef.Effects.ReverbTime = theme->reverbTime;
			}

			m_ActiveTheme = theme;

			// Engine::PlayMusic() uses a mutex, so let's submit it in a deteached thread to avoid blocking
			std::thread submitThread([this, musicDef]() {
				m_BassEngine->PlayMusic(musicDef);
			});
			submitThread.detach();
		}

		zCMusicTheme* GetActiveTheme() override
		{
			return m_ActiveTheme;
		}

		void DoMusicUpdate() override
		{
			m_DirectMusic->DoMusicUpdate();
		}

		void Mute() override
		{
			NH::Log::Debug("CMusicSys_Bass", Union::StringUTF8("Mute"));
			m_BassEngine->SetVolume(0.0f);
			m_DirectMusic->Mute();
		}

		void Stop() override
		{
			NH::Log::Debug("CMusicSys_Bass", Union::StringUTF8("Stop"));
			m_ActiveTheme = nullptr;
			m_BassEngine->StopMusic();
			m_DirectMusic->Stop();
		}

		float GetVolume() const override
		{
			return m_BassEngine->GetVolume();
		}

		void SetVolume(float volume) override
		{
			m_BassEngine->SetVolume(volume);
			m_DirectMusic->SetVolume(volume);
		}

		int IsAvailable(zSTRING const& id) override
		{
			return (NH::Bass::Options->CreateMainParserCMusicTheme && parser->GetSymbol(id) != nullptr) || parserMusic->GetSymbol(id) != nullptr;
		}
	};
}