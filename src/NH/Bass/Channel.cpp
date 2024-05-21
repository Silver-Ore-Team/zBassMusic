#include "Channel.h"
#include "NH/Bass/Engine.h"
#include "Options.h"

namespace NH::Bass
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
			log->Error("Could not create stream: {0}\n  error: {1}\n  at {2}:{3}",
					m_Music.Filename, Engine::ErrorCodeToString(BASS_ErrorGetCode()),
					__FILE__, __LINE__);
			return;
		}

		BASS_ChannelStart(m_Stream);

		log->Info("Channel started: {0}", m_Music.Filename);

		if (Options->ForceFadeTransition)
		{
			log->Debug("BASSENGINE.ForceFadeTransition is set, forcing TransitionType::FADE");
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
			log->Trace("Loop set {0}", music.Filename);
		}

		if (!Options->ForceDisableReverb && m_Music.Effects.Reverb)
		{
			HFX fx = BASS_ChannelSetFX(m_Stream, BASS_FX_DX8_REVERB, 1);
			BASS_DX8_REVERB params{ 0, m_Music.Effects.ReverbMix, m_Music.Effects.ReverbTime, 0.001f };
			if (!BASS_FXSetParameters(fx, (void*)&params))
			{
				log->Error("Could not set reverb FX: {0}\n  error: {1}\n  at {2}:{3}",
						m_Music.Filename, Engine::ErrorCodeToString(BASS_ErrorGetCode()),
						__FILE__, __LINE__);
			}
			log->Trace("Reverb set: {0}", m_Music.Filename);
		}

		if (m_Music.EndTransition.Type != TransitionType::NONE)
		{
			const QWORD length = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
			const QWORD transitionBytes = BASS_ChannelSeconds2Bytes(m_Stream, m_Music.EndTransition.Duration / 1000.0f);
			const QWORD offset = length - transitionBytes;
			BASS_ChannelSetSync(m_Stream, BASS_SYNC_POS, offset, OnTransitionSync, this);
			log->Trace("SyncTransitionPos set: {0}", m_Music.Filename);
		}

		BASS_ChannelSetSync(m_Stream, BASS_SYNC_END, 0, OnEndSync, this);
		log->Trace("SyncEnd set: {0}", m_Music.Filename);

		m_Status = ChannelStatus::PLAYING;
		m_EventManager.DispatchEvent(EventType::MUSIC_CHANGE, m_Music);
	}

	double Channel::CurrentPosition() const
	{
		if (m_Stream > 0)
		{
			return BASS_ChannelBytes2Seconds(m_Stream, BASS_ChannelGetPosition(m_Stream, BASS_POS_BYTE));
		}
		return -1;
	}

	double Channel::CurrentLength() const
	{
		if (m_Stream > 0)
		{
			return BASS_ChannelBytes2Seconds(m_Stream, BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE));
		}
		return -1;
	}

	const MusicDef& Channel::CurrentMusic() const
	{
		return m_Music;
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
			_this->m_EventManager.DispatchEvent(EventType::MUSIC_TRANSITION, _this->m_Music,
					_this->m_Music.EndTransition.Duration);
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
