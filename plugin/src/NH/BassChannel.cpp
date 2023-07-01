#include "BassChannel.h"
#include <NH/Union.h>
#include <NH/Bass.h>
#include <NH/BassOptions.h>

namespace NH
{
	namespace Bass
	{
		void Channel::Play(const MusicDef& music, const MusicFile* file)
		{
			if (m_Stream > 0)
			{
				BASS_ChannelFree(m_Stream);
			}

			m_Music = music;
			m_Stream = BASS_StreamCreateFile(true, file->Buffer.data(), 0, file->Buffer.size(), 0);
			if (!m_Stream)
			{
				NH::Log::Error("BassChannel", Union::StringUTF8("Could not create stream: ") + m_Music.Filename);
				NH::Log::Error("BassChannel", Union::StringUTF8("Could not create stream: ") + Engine::ErrorCodeToString(BASS_ErrorGetCode()));
				return;
			}

			BASS_ChannelStart(m_Stream);

			Log::Debug("BassChannel", Union::StringUTF8("Channel started: ") + m_Music.Filename);

			if (Options->ForceFadeTransition)
			{
				Log::Info("BassChannel", Union::StringUTF8("BASSENGINE.ForceFadeTransition is set, forcing TransitionType::FADE"));
				m_Music.StartTransition.Type = TransitionType::FADE;
				m_Music.EndTransition.Type = TransitionType::FADE;
			}

			if (m_Music.StartTransition.Type == TransitionType::FADE)
			{
				BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_VOL, 0.0f);
				BASS_ChannelSlideAttribute(m_Stream, BASS_ATTRIB_VOL, m_Music.Volume, m_Music.StartTransition.Duration);
			}
			else
			{
				BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_VOL, m_Music.Volume);
			}

			if (m_Music.Loop)
			{
				BASS_ChannelFlags(m_Stream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
				Log::Debug("BassChannel", Union::StringUTF8("Loop set: ") + music.Filename);
			}

			if (!Options->ForceDisableReverb && m_Music.Effects.Reverb)
			{
				HFX fx = BASS_ChannelSetFX(m_Stream, BASS_FX_DX8_REVERB, 1);
				BASS_DX8_REVERB params{ 0, m_Music.Effects.ReverbMix, m_Music.Effects.ReverbTime, 0.001f };
				if (!BASS_FXSetParameters(fx, (void*)&params))
				{
					Log::Error("BassChannel", Union::StringUTF8("Could not set reverb FX: ") + m_Music.Filename);
					Log::Error("BassChannel", Union::StringUTF8("Could not set reverb FX: ") + Engine::ErrorCodeToString(BASS_ErrorGetCode()));
				}
				Log::Debug("BassChannel", Union::StringUTF8("Reverb set: ") + m_Music.Filename);
			}

			if (m_Music.EndTransition.Type != TransitionType::NONE)
			{
				const QWORD length = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
				const QWORD transitionBytes = BASS_ChannelSeconds2Bytes(m_Stream, m_Music.EndTransition.Duration / 1000.0f);
				const QWORD offset = length - transitionBytes;
				BASS_ChannelSetSync(m_Stream, BASS_SYNC_POS, offset, OnTransitionSync, this);
				Log::Debug("BassChannel", Union::StringUTF8("SyncTransitionPos set: ") + m_Music.Filename);
			}

			BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, 0, OnEndSync, this);
			Log::Debug("BassChannel", Union::StringUTF8("SyncEnd set: ") + m_Music.Filename);

			m_Status = ChannelStatus::PLAYING;
			m_EventManager.DispatchEvent(EventType::MUSIC_CHANGE, m_Music);
		}

		void Channel::Stop()
		{
			if (m_Stream > 0)
			{
				if (m_Music.EndTransition.Type == TransitionType::FADE)
				{
					BASS_ChannelSlideAttribute(m_Stream, BASS_ATTRIB_VOL, 0.0f, m_Music.EndTransition.Duration);
					BASS_ChannelSetSync(m_Stream, BASS_SYNC_SLIDE, 0, OnVolumeSlideSync, this);
					m_Status = ChannelStatus::FADING_OUT;
				}
				else
				{
					BASS_ChannelFree(m_Stream);
					m_Stream = 0;
					m_Status = ChannelStatus::AVAILABLE;
				}
			}
		}

		void Channel::OnTransitionSync(HSYNC, DWORD channel, DWORD data, void* userData)
		{
			const auto _this = static_cast<Channel*>(userData);
			if (_this->m_Stream == channel)
			{
				_this->m_EventManager.DispatchEvent(EventType::MUSIC_TRANSITION, _this->m_Music, _this->m_Music.EndTransition.Duration);
			}
		}

		void Channel::OnEndSync(HSYNC, DWORD channel, DWORD data, void* userData)
		{
			const auto _this = static_cast<Channel*>(userData);
			if (_this->m_Stream == channel)
			{
				_this->m_EventManager.DispatchEvent(EventType::MUSIC_END, _this->m_Music);
				if (!_this->m_Music.Loop)
				{
					_this->m_Status = ChannelStatus::AVAILABLE;
				}
			}
		}

		void Channel::OnVolumeSlideSync(HSYNC, DWORD channel, DWORD data, void* userData)
		{
			const auto _this = static_cast<Channel*>(userData);
			if (_this->m_Stream == channel && _this->m_Status == ChannelStatus::FADING_OUT)
			{
				float volume;
				BASS_ChannelGetAttribute(channel, BASS_ATTRIB_VOL, &volume);
				if (volume < DBL_EPSILON)
				{
					BASS_ChannelFree(channel);
					_this->m_Stream = 0;
					_this->m_Status = ChannelStatus::AVAILABLE;
				}
			}
		}

	}
}