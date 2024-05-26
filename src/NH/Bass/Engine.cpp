#include "Options.h"
#include "Engine.h"

#include <bassmidi.h>
#include <bassopus.h>
#include <bassflac.h>

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

        m_TransitionScheduler.Update(*this);
        m_CommandQueue.Update(*this);

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

    MusicManager& Engine::GetMusicManager()
    {
        return m_MusicManager;
    }

    CommandQueue& Engine::GetCommandQueue()
    {
        return m_CommandQueue;
    }

    void Engine::StopMusic()
    {
        if (!m_Initialized)
        {
            return;
        }

        for (const auto& channel: m_Channels)
        {
            channel->Stop();
        }

        m_ActiveChannel = nullptr;
    }

    Engine::Engine()
    {
        HPLUGIN midiPlugin = BASS_PluginLoad("bassmidi.dll", 0);
        if (midiPlugin) { BASS_PluginEnable(midiPlugin, true); }
        else {
            log->Warning("Could not load BASSMIDI plugin. Make sure that bassmidi.dll is in the working directory or Autorun.");
            log->Warning("BASSMIDI plugin is required for MIDI metadata. Engine may crash without it.");
        }

        HPLUGIN opusPlugin = BASS_PluginLoad("bassopus.dll", 0);
        if (opusPlugin) { BASS_PluginEnable(opusPlugin, true); }
        else { log->Warning("Could not load BASSOPUS plugin (bassopus.dll). Opus files won't play."); }

        HPLUGIN flacPlugin = BASS_PluginLoad("bassflac.dll", 0);
        if (flacPlugin) { BASS_PluginEnable(flacPlugin, true); }
        else { log->Warning("Could not load BASSFLAC plugin (bassflac.dll). FLAC files won't play."); }

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
            m_Channels.emplace_back(std::make_shared<Channel>(i, m_EventManager));
        }

        log->Info("Initialized with device: {0}", deviceIndex);
    }

    std::shared_ptr<Channel> Engine::FindAvailableChannel()
    {
        for (auto channel: m_Channels)
        {
            if (channel->IsAvailable())
            {
                return channel;
            }
        }

        return nullptr;
    }

    Union::StringUTF8 Engine::ErrorCodeToString(const int code)
    {
        // @formatter:off
        static String map[] = { "BASS_OK", "BASS_ERROR_MEM", "BASS_ERROR_FILEOPEN", "BASS_ERROR_DRIVER", "BASS_ERROR_BUFLOST", "BASS_ERROR_HANDLE", "BASS_ERROR_FORMAT", "BASS_ERROR_POSITION", "BASS_ERROR_INIT", "BASS_ERROR_START", "BASS_ERROR_SSL", "BASS_ERROR_REINIT", "BASS_ERROR_ALREADY", "BASS_ERROR_NOTAUDIO", "BASS_ERROR_NOCHAN", "BASS_ERROR_ILLTYPE", "BASS_ERROR_ILLPARAM", "BASS_ERROR_NO3D", "BASS_ERROR_NOEAX", "BASS_ERROR_DEVICE", "BASS_ERROR_NOPLAY", "BASS_ERROR_FREQ", "BASS_ERROR_NOTFILE", "BASS_ERROR_NOHW", "BASS_ERROR_EMPTY", "BASS_ERROR_NONET", "BASS_ERROR_CREATE", "BASS_ERROR_NOFX", "BASS_ERROR_NOTAVAIL", "BASS_ERROR_DECODE", "BASS_ERROR_DX", "BASS_ERROR_TIMEOUT", "BASS_ERROR_FILEFORM", "BASS_ERROR_SPEAKER", "BASS_ERROR_VERSION", "BASS_ERROR_CODEC", "BASS_ERROR_ENDED", "BASS_ERROR_BUSY", "BASS_ERROR_UNSTREAMABLE", "BASS_ERROR_PROTOCOL", "BASS_ERROR_DENIED", "BASS_ERROR_UNKNOWN" };
        // @formatter:on
        return map[code];
    }

    Logger* ChangeZoneCommand::log = CreateLogger("zBassMusic::ChangeZoneCommand");
    Logger* PlayThemeCommand::log = CreateLogger("zBassMusic::PlayThemeCommand");
    Logger* ScheduleThemeChangeCommand::log = CreateLogger("zBassMusic::ScheduleThemeChangeCommand");

    CommandResult ChangeZoneCommand::Execute(Engine& engine)
    {
        log->Trace("Executing ChangeZoneCommand for zone {0}", m_Zone);

        const auto themes = engine.GetMusicManager().GetThemesForZone(m_Zone);
        if (themes.empty())
        {
            log->Warning("No themes found for zone {0}", m_Zone);
            return CommandResult::DONE;
        }
        engine.GetCommandQueue().AddCommandDeferred(std::make_shared<ScheduleThemeChangeCommand>(themes[0].first));
        return CommandResult::DONE;
    }

    CommandResult PlayThemeCommand::Execute(Engine& engine)
    {
        log->Trace("Executing PlayThemeCommand for theme {0}", m_ThemeId);

        auto theme = engine.GetMusicManager().GetTheme(m_ThemeId);
        if (!theme->IsAudioFileReady(AudioFile::DEFAULT))
        {
            log->Trace("Theme {0} is not ready", m_ThemeId);
            m_RetryCount++;
            if (m_RetryCount > 10000)
            {
                log->Error("Theme {0} was not ready after 10000 retries (roughly 600ms @ 60 FPS), removing it from queue", m_ThemeId);
                return CommandResult::DONE;
            }
            return CommandResult::RETRY;
        }

        auto channel = engine.FindAvailableChannel();
        if (!channel)
        {
            log->Error("No available channels");
            return CommandResult::DEFER;
        }

        if (engine.m_ActiveChannel) { engine.m_ActiveChannel->Stop(); }
        channel->Play(theme, m_AudioId);
        engine.m_ActiveChannel = channel;

        return CommandResult::DONE;
    }

    CommandResult ScheduleThemeChangeCommand::Execute(Engine& engine)
    {
        log->Trace("Executing ScheduleThemeChangeCommand for theme {0}", m_ThemeId);
        auto theme = engine.GetMusicManager().GetTheme(m_ThemeId);
        engine.GetTransitionScheduler().Schedule(engine.m_ActiveChannel, theme);
        return CommandResult::DONE;
    }
}