#include <vector>

namespace GOTHIC_NAMESPACE
{
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
			parserMusic->CreateInstance(identifier, (void*)(&(theme->fileName)));
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
					NH::Log::Info("CMusicSys_Bass", Union::StringUTF8::Format("Music file: ") + file->GetFullPath().ToChar());
					file->Open(false);
					size_t size = file->Size();
					std::vector<char> buffer(size);
					size_t read = file->Read(buffer.data(), size);
					NH::Log::Debug("CMusicSys_Bass", Union::StringUTF8::Format("Music size: ") + Union::StringUTF8(size));
					NH::Log::Debug("CMusicSys_Bass", Union::StringUTF8::Format("Music read: ") + Union::StringUTF8(read));
					file->Close();
					m_BassEngine->LoadMusicFile(theme->fileName.ToChar(), buffer);
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

			NH::Bass::MusicDef musicDef{};
			musicDef.Filename = theme->fileName.ToChar();
			musicDef.Name = theme->name.ToChar();
			musicDef.Loop = theme->loop;
			musicDef.Volume = theme->vol;

			if (theme->trType == zMUS_TR_INTRO || theme->trType == zMUS_TR_ENDANDINTRO)
			{
				musicDef.StartTransition.Type = NH::Bass::TransitionType::FADE;
				musicDef.StartTransition.Duration = 2000.0f; // @todo: move to config
			}

			if (theme->trType == zMUS_TR_END || theme->trType == zMUS_TR_ENDANDINTRO)
			{
				musicDef.EndTransition.Type = NH::Bass::TransitionType::FADE;
				musicDef.EndTransition.Duration = 2000.0f; // @todo: move to config
			}

			if (m_DirectMusic->prefs.globalReverbEnabled)
			{
				musicDef.Effects.Reverb = true;
				musicDef.Effects.ReverbMix = theme->reverbMix;
				musicDef.Effects.ReverbTime = theme->reverbTime;
			}

			m_BassEngine->PlayMusic(musicDef);

			m_ActiveTheme = theme;
		}

		zCMusicTheme* GetActiveTheme() override
		{
			return m_ActiveTheme;
		}

		void DoMusicUpdate() override
		{
			m_BassEngine->Update(ztimer->lastTimer);
			m_DirectMusic->DoMusicUpdate();
		}

		void Mute() override
		{
			m_BassEngine->SetVolume(0.0f);
			m_DirectMusic->Mute();
		}

		void Stop() override
		{
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
			return parserMusic->GetSymbol(id) != nullptr;
		}
	};
}