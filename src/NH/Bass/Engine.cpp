#include "Options.h"
#include "Engine.h"

#include <bassopus.h>

#include <algorithm>

namespace NH::Bass
{
    Logger* Engine::log = CreateLogger("zBassMusic::Engine");

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
        const auto now = std::chrono::system_clock::now();
        const uint64_t delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimestamp).count();
        lastTimestamp = now;

        if (m_ActiveTheme && !m_ActiveTheme->IsPlaying(*NH::Bass::Engine::GetInstance()))
        {
            m_ActiveTheme->Play(*NH::Bass::Engine::GetInstance());
            log->Debug("Re-playing active theme: {0}", m_ActiveTheme->GetName().c_str());
        }

        m_CommandQueue.Update(*this);

        BASS_Update(static_cast<int32_t>(delta));
        GetEM().Update();
    }

    std::shared_ptr<IChannel> Engine::AcquireFreeChannel()
    {
        for (auto channel: m_Channels)
        {
            if (channel->IsAvailable())
            {
                channel->Acquire();
                return channel;
            }
        }

        log->Info("No channel available. Creating a new one.");
        if (m_Channels.size() > 32)
        {
            log->Warning("There are more than 32 channels. Report this to the developers because some channels may not be released.");
        }
        if (m_Channels.size() > 128)
        {
            log->Error("There are more than 128 channels. We are stopping this now because it's too much and there is definitely some leak.");
            throw std::runtime_error("Too many channels");
        }

        return m_Channels.emplace_back(std::make_shared<Channel>(m_Channels.size()));
    }

    void Engine::ReleaseChannel(const std::shared_ptr<IChannel>& channel)
    {
        channel->Release();
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
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, static_cast<uint32_t>(10000 * m_MasterVolume));
    }

    float Engine::GetVolume() const
    {
        return m_MasterVolume;
    }

    std::shared_ptr<MusicTheme> Engine::GetActiveTheme()
    {
        return m_ActiveTheme;
    }

    void Engine::SetActiveTheme(const std::shared_ptr<MusicTheme>& theme)
    {
        m_ActiveTheme = theme;
    }

    EventManager& Engine::GetEM()
    {
        return m_EventManager;
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
        if (!m_Initialized) { return; }
        if (m_ActiveTheme)
        {
            m_ActiveTheme->Stop(*this);
            m_ActiveTheme = nullptr;
        }
    }

    Engine::Engine()
    {
        if (const HPLUGIN midiPlugin = BASS_PluginLoad("bassmidi.dll", 0)) { BASS_PluginEnable(midiPlugin, true); }
        else {
            log->Warning("Could not load BASSMIDI plugin. Make sure that bassmidi.dll is in the working directory or Autorun.");
            log->Warning("BASSMIDI plugin is required for MIDI metadata. Engine may crash without it.");
        }

        if (const HPLUGIN opusPlugin = BASS_PluginLoad("bassopus.dll", 0)) { BASS_PluginEnable(opusPlugin, true); }
        else { log->Warning("Could not load BASSOPUS plugin (bassopus.dll). Opus files won't play."); }

        if (const HPLUGIN flacPlugin = BASS_PluginLoad("bassflac.dll", 0)) { BASS_PluginEnable(flacPlugin, true); }
        else { log->Warning("Could not load BASSFLAC plugin (bassflac.dll). FLAC files won't play."); }

        size_t deviceIndex = 0;
        BASS_DEVICEINFO deviceInfo;
        for (size_t i = 1; BASS_GetDeviceInfo(i, &deviceInfo); i++)
        {
            const bool enabled = deviceInfo.flags & BASS_DEVICE_ENABLED;
            const bool isDefault = deviceInfo.flags & BASS_DEVICE_DEFAULT;
            log->Trace("Available device: {0} {1} {2}", deviceInfo.name, enabled ? "enabled" : "disabled", isDefault ? "default" : "");
            if (enabled && isDefault)
            {
                deviceIndex = i;
            }
        };

        BASS_SetDevice(deviceIndex);
        BASS_GetDeviceInfo(deviceIndex, &deviceInfo);
        log->Info("Device Name: {0}", deviceInfo.name);

        m_Initialized = BASS_Init(static_cast<int32_t>(deviceIndex), 44100, 0, nullptr, nullptr);
        if (!m_Initialized)
        {
            log->Error("Could not initialize BASS using BASS_Init\n  {0}\n  at {1}:{2}",
                       ErrorCodeToString(BASS_ErrorGetCode()), __FILE__, __LINE__);
            return;
        }

        BASS_INFO info;
        BASS_GetInfo(&info);
        log->Info("Sample Rate: {0} Hz", info.freq);

        static constexpr size_t Channels_Max = 16;
        m_Channels.clear();
        for (size_t i = 0; i < Channels_Max; i++)
        {
            m_Channels.emplace_back(std::make_shared<Channel>(i));
        }
    }

    Union::StringUTF8 Engine::ErrorCodeToString(const int code)
    {
        // @formatter:off
        static const String map[] = { "BASS_OK", "BASS_ERROR_MEM", "BASS_ERROR_FILEOPEN", "BASS_ERROR_DRIVER", "BASS_ERROR_BUFLOST", "BASS_ERROR_HANDLE", "BASS_ERROR_FORMAT", "BASS_ERROR_POSITION", "BASS_ERROR_INIT", "BASS_ERROR_START", "BASS_ERROR_SSL", "BASS_ERROR_REINIT", "BASS_ERROR_ALREADY", "BASS_ERROR_NOTAUDIO", "BASS_ERROR_NOCHAN", "BASS_ERROR_ILLTYPE", "BASS_ERROR_ILLPARAM", "BASS_ERROR_NO3D", "BASS_ERROR_NOEAX", "BASS_ERROR_DEVICE", "BASS_ERROR_NOPLAY", "BASS_ERROR_FREQ", "BASS_ERROR_NOTFILE", "BASS_ERROR_NOHW", "BASS_ERROR_EMPTY", "BASS_ERROR_NONET", "BASS_ERROR_CREATE", "BASS_ERROR_NOFX", "BASS_ERROR_NOTAVAIL", "BASS_ERROR_DECODE", "BASS_ERROR_DX", "BASS_ERROR_TIMEOUT", "BASS_ERROR_FILEFORM", "BASS_ERROR_SPEAKER", "BASS_ERROR_VERSION", "BASS_ERROR_CODEC", "BASS_ERROR_ENDED", "BASS_ERROR_BUSY", "BASS_ERROR_UNSTREAMABLE", "BASS_ERROR_PROTOCOL", "BASS_ERROR_DENIED", "BASS_ERROR_UNKNOWN" };
        // @formatter:on
        return map[code];
    }

    std::string Engine::GetCurrentZone()
    {
        return m_CurrentZone;
    }

    void Engine::SetCurrentZone(const std::string& zone)
    {
        m_CurrentZone = zone;
    }
}