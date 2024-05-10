namespace GOTHIC_NAMESPACE
{
	namespace BassEvent
	{
		void Event_OnEnd(const NH::Bass::MusicDef& musicDef, int data, void* userData)
		{
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnEnd");
            log->Trace("{0}", musicDef.Filename);

			zSTRING filename{ musicDef.Filename.ToChar() };
			zSTRING name{ musicDef.Name.ToChar() };
			
			for (int i = 0; i < Globals->Event_OnEnd_Functions.GetNumInList(); i++)
			{
				const int funcId = Globals->Event_OnEnd_Functions[i];
				Globals->BassMusic_EventThemeFilename = filename;
				Globals->BassMusic_EventThemeID = name;
				parser->CallFunc(funcId);
			}
		}

		void Event_OnTransition(const NH::Bass::MusicDef& musicDef, int data, void* userData)
		{
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnTransition");
            log->Trace("{0}, {1} ms", musicDef.Filename, data);

			zSTRING filename{ musicDef.Filename.ToChar() };
			zSTRING name{ musicDef.Name.ToChar() };

			for (int i = 0; i < Globals->Event_OnTransition_Functions.GetNumInList(); i++)
			{
				const int funcId = Globals->Event_OnTransition_Functions[i];
				Globals->BassMusic_EventThemeFilename = filename;
				Globals->BassMusic_EventThemeID = name;
				parser->CallFunc(funcId, data);
			}
		}

		void Event_OnChange(const NH::Bass::MusicDef& musicDef, int data, void* userData)
		{
            static NH::Logger* log = NH::CreateLogger("zBassMusic::Event_OnChange");
            log->Trace("{0}", musicDef.Filename);

			zSTRING filename{ musicDef.Filename.ToChar() };
			zSTRING name{ musicDef.Name.ToChar() };

			Globals->BassMusic_ActiveThemeFilename = filename;
			Globals->BassMusic_ActiveThemeID = name;

			for (int i = 0; i < Globals->Event_OnChange_Functions.GetNumInList(); i++)
			{
				const int funcId = Globals->Event_OnChange_Functions[i];
				Globals->BassMusic_EventThemeFilename = filename;
				Globals->BassMusic_EventThemeID = name;
				parser->CallFunc(funcId);
			}
		}
	}

	class CMusicSys_Bass : public zCMusicSystem
	{
	private:
        NH::Logger* log = NH::CreateLogger("zBassMusic::CMusicSys_Bass");
		NH::Bass::Engine* m_BassEngine;
		zCMusicSys_DirectMusic* m_DirectMusic;
		zCMusicTheme* m_ActiveTheme = nullptr;

		static bool IsDirectMusicFormat(const zSTRING& file)
		{
			static constexpr std::string_view validExt{ ".sgt" };

			if (file.Length() < validExt.size())
			{
				return false;
			}

			const auto fileLen = static_cast<size_t>(file.Length());
			const auto extStart = fileLen - validExt.size();
			const std::string_view fileExt{ std::next(file.ToChar(),extStart), validExt.size() };

			auto toLowerSimple = [](const char t_char)
			{
				return t_char >= 'A' && t_char <= 'Z'
					? t_char + ('a' - 'A')
					: t_char;
			};

			return std::ranges::equal(fileExt | std::views::transform(toLowerSimple), validExt);
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
			if (s_musicSystemDisabled || id.IsEmpty())
			{
				return nullptr;
			}

            log->Trace("LoadThemeByScript: {0}", id.ToChar());

			zSTRING identifier = id;
			if (m_ActiveTheme && identifier.Upper() == m_ActiveTheme->name)
			{
				return m_ActiveTheme;
			}

			zCMusicTheme* theme = new zCMusicTheme;
			
			if (!(NH::Bass::Options->CreateMainParserCMusicTheme && parser->CreateInstance(identifier, &theme->fileName)))
			{
				parserMusic->CreateInstance(identifier, &theme->fileName);
			}

			if (IsDirectMusicFormat(theme->fileName))
			{
				delete theme;
				theme = m_DirectMusic->LoadThemeByScript(id);
			}
			else
			{
				theme->name = identifier;

				zoptions->ChangeDir(DIR_MUSIC);
				std::unique_ptr<zFILE> file{ zfactory->CreateZFile(theme->fileName) };

				if (file->Exists())
				{
					NH::Bass::MusicFile& musicFileRef = m_BassEngine->CreateMusicBuffer(theme->fileName.ToChar());
					if (!musicFileRef.Ready && !musicFileRef.Loading)
					{
                        log->Trace("Loading music: {0}", file->GetFullPath().ToChar());

						const auto error = file->Open(false);

						if (error == 0)
						{
							musicFileRef.Loading = true;

							std::thread([loadingStart = std::chrono::system_clock::now(), this](std::unique_ptr<zFILE> myFile, NH::Bass::MusicFile* myMusicPtr) {

								zSTRING path = myFile->GetFullPath();
								const long size = myFile->Size();
								
								myMusicPtr->Buffer.resize(static_cast<size_t>(size));
								const long read = myFile->Read(myMusicPtr->Buffer.data(), size);

								if (read == size)
								{
									myMusicPtr->Ready = true;
                                    log->Trace("Music ready: {0}, size = {1}", path.ToChar(), read);
								}

								myMusicPtr->Loading = false;
								myFile->Close();
											
								auto loadingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - loadingStart).count();

                                log->Trace("Loading done: {0}, time = {1}", path.ToChar(), loadingTime);
                            }, std::move(file), &musicFileRef).detach();
						}
						else
						{
                            log->Error("Could not open file {0}\n  at {1}{2}", theme->fileName.ToChar(), __FILE__, __LINE__);
						}
					}
				}
				else
				{
                    log->Error("Could not find file {0}\n  at {1}{2}", theme->fileName.ToChar(), __FILE__, __LINE__);
				}
			}

			return theme;
		}

		void PlayThemeByScript(zSTRING const& id, int manipulate, int* done) override
		{
			if (s_musicSystemDisabled || id.IsEmpty())
			{
				return;
			}

            log->Trace("PlayThemeByScript: {0}", id.ToChar());

			if (Globals->FullScriptControl)
			{
                log->Trace("PlayThemeByScript skipped because FullScriptControl is enabled.");
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
            log->Trace("PlayTheme: {0}", theme->fileName.ToChar());

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
            log->Trace("Mute");
			m_BassEngine->SetVolume(0.0f);
			m_DirectMusic->Mute();
		}

		void Stop() override
		{
            log->Trace("Stop");
			m_ActiveTheme = nullptr;
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