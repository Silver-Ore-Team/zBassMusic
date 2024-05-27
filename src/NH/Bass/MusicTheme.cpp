#include "MusicTheme.h"
#include "EngineCommands.h"

#include <Union/VDFS.h>
#include <NH/Bass/Command.h>
#include <NH/Bass/EventManager.h>

namespace NH::Bass
{
    HashString AudioFile::DEFAULT = "DEFAULT";
    AudioEffects AudioEffects::None = {};
    MusicTheme MusicTheme::None = MusicTheme("<None>");
    Logger* MusicTheme::log = CreateLogger("zBassMusic::MusicTheme");

    MusicTheme::MusicTheme(const String& name) : m_Name(name), m_TransitionInfo{ m_Name } {}

    void MusicTheme::SetAudioFile(HashString type, const String& filename)
    {
        m_AudioFiles.emplace(std::make_pair(type, AudioFile{}));
        m_AudioFiles[type].Filename = filename;
        m_AudioFiles[type].Status = AudioFile::StatusType::NOT_LOADED;
    }

    void MusicTheme::SetAudioEffects(HashString type, const std::function<void(AudioEffects&)>& effectsSetter)
    {
        if (!m_AudioFiles.contains(type))
        {
            m_AudioEffects[type] = AudioEffects{};
        }
        effectsSetter(m_AudioEffects[type]);
        if (m_AudioEffects[type].FadeOut.Active)
        {
            m_TransitionInfo.AddTransitionEffect(TransitionEffect::CROSSFADE, m_AudioEffects[type].FadeOut.Duration);
        }
    }

    void MusicTheme::AddZone(HashString zone)
    {
        m_Zones.emplace_back(zone);
    }

    void MusicTheme::AddMidiFile(HashString type, std::shared_ptr<MidiFile> midiFile)
    {
        m_MidiFiles.emplace(std::make_pair(HashString(type), midiFile));
        m_MidiFiles[type]->LoadMidiFile(Executors.IO, [this, type](MidiFile& midi) {
            m_TransitionInfo.AddMidiFile(midi, type);
        });
    }

    void MusicTheme::LoadAudioFiles(Executor& executor)
    {
        for (auto& [type, audioFile]: m_AudioFiles)
        {
            if (audioFile.Status == AudioFile::StatusType::NOT_LOADED)
            {
                audioFile.Status = AudioFile::StatusType::LOADING;
                executor.AddTask([type, this]() {
                    int systems = VDF_VIRTUAL | VDF_PHYSICAL;
                    const Union::VDFS::File* file = Union::VDFS::GetDefaultInstance().GetFile(m_AudioFiles[type].Filename, systems);
                    if (!file)
                    {
                        m_AudioFiles[type].Status = AudioFile::StatusType::FAILED;
                        m_AudioFiles[type].Error = "File not found";
                        return;
                    }

                    Union::Stream* stream = file->Open();
                    m_AudioFiles[type].Buffer.resize(stream->GetSize());
                    stream->Read(m_AudioFiles[type].Buffer.data(), m_AudioFiles[type].Buffer.size());
                    stream->Close();

                    m_AudioFiles[type].Status = AudioFile::StatusType::READY;
                });
            }
        }
    }

    void MusicTheme::Schedule(IEngine& engine, const std::shared_ptr<MusicTheme>& currentTheme)
    {
        currentTheme->Transition(engine, *this);
    }

    void MusicTheme::Transition(IEngine& engine, MusicTheme& nextTheme)
    {
        if (nextTheme.GetName() == GetName()) { return; }
        auto channel = GetAcquiredChannel();
        if (!channel) channel = m_AcquiredChannels.emplace_back(engine.AcquireFreeChannel());
        const auto& transition = m_TransitionInfo.GetTransition(nextTheme.GetName());
        std::optional<Transition::TimePoint> timePoint = transition.NextAvailableTimePoint(channel->Position());

        log->Debug("Transition {0} to {1}", GetName(), nextTheme.GetName());
        log->PrintRaw(LoggerLevel::Trace, transition.ToString());

        const std::function<void()>& playJingle = [&engine, &transition, this]() {
            if (transition.Jingle)
            {
                auto channel = engine.AcquireFreeChannel();
                auto result = channel->PlayInstant(transition.Jingle->GetAudioFile(AudioFile::DEFAULT));
                if (result) { channel->OnAudioEnds(CreateSyncHandler([channel]() { channel->Release(); })); }
                else { channel->Release(); }
            }
        };

        if (timePoint && channel->IsPlaying())
        {
            log->Trace("OnPosition: {0}", String(timePoint->Start));
            channel->OnPosition(timePoint->Start, CreateSyncHandler([&engine, &transition, this]() {
                Stop(engine, transition);
            }));
            channel->OnPosition(timePoint->Start + transition.JingleDelay, playJingle);
            channel->OnPosition(timePoint->NextStart, CreateSyncHandler([&engine, &nextTheme, &transition, timePoint]() {
                nextTheme.Play(engine, transition, timePoint);
            }));
        }
        else
        {
            Stop(engine, transition);
            playJingle();
            nextTheme.Play(engine, transition);
        }
    }

    void MusicTheme::Play(IEngine& engine) { Play(engine, Transition::EMPTY); }
    void MusicTheme::Play(IEngine& engine, const struct Transition& transition, std::optional<Transition::TimePoint> timePoint)
    {
        if (!ReadyToPlay(engine, AudioFile::DEFAULT)) { return; }

        auto channel = m_AcquiredChannels.emplace_back(engine.AcquireFreeChannel());
        auto& effects = GetAudioEffects(AudioFile::DEFAULT);
        auto effect = timePoint.has_value() ? timePoint.value().NextEffect : transition.Effect;
        auto result = channel->PlayInstant(GetAudioFile(AudioFile::DEFAULT));
        if (!result)
        {
            ReleaseChannels();
            return;
        }

        if (effects.ReverbDX8.Active)
        {
            channel->SetDX8ReverbEffect(effects.ReverbDX8.Mix, effects.ReverbDX8.Time, 0, 0.001f);
        }

        if (effect == TransitionEffect::CROSSFADE)
        {
            channel->SetVolume(0.0f);
            channel->SlideVolume(1.0f, timePoint.has_value() ? (uint32_t)timePoint.value().Duration * 1000 : (uint32_t)effects.FadeIn.Duration);
        }
        else { channel->SetVolume(effects.Volume.Active ? effects.Volume.Volume : 1.0f); }

        if (effects.FadeOut.Active)
        {
            channel->BeforeAudioEnds(effects.FadeOut.Duration, CreateSyncHandler([this, &engine](double timeLeft) -> void {
                engine.GetEM().DispatchEvent(MusicTransitionEvent{ this, AudioFile::DEFAULT, static_cast<float>(timeLeft) });
            }));
        }

        channel->OnAudioEnds(CreateSyncHandler([this, &engine]() -> void { engine.GetEM().DispatchEvent(MusicEndEvent{ this, AudioFile::DEFAULT }); }));
        engine.GetEM().DispatchEvent(MusicChangeEvent{ this, AudioFile::DEFAULT });
    }

    void MusicTheme::Stop(IEngine& engine) { Stop(engine, m_TransitionInfo.GetDefaultTransition()); }
    void MusicTheme::Stop(IEngine& engine, const struct Transition& transition)
    {
        auto channel = GetAcquiredChannel();
        if (!channel) { ReleaseChannels(); };
        std::optional<Transition::TimePoint> timePoint = transition.NextAvailableTimePoint(channel->Position());
        auto effect = timePoint.has_value() ? timePoint.value().Effect : transition.Effect;
        if (effect == TransitionEffect::CROSSFADE)
        {
            uint32_t time = timePoint.has_value() ? (uint32_t)timePoint.value().Duration * 1000 : (uint32_t)transition.EffectDuration;
            log->Trace("Fadeout, time: {0}", time);
            channel->SlideVolume(0.0f, time, CreateSyncHandler([channel, this]() -> void {
                log->Trace("Fadeout done");
                channel->StopInstant();
                ReleaseChannels();
            }));
        }
        else
        {
            channel->StopInstant();
            ReleaseChannels();
        }
    }

    bool MusicTheme::ReadyToPlay(IEngine& engine, HashString audio)
    {
        const auto& file = GetAudioFile(AudioFile::DEFAULT);
        if (file.Status == AudioFile::StatusType::FAILED)
        {
            log->Error("Trying to play a music theme that has failed to load: {0}", file);
            return false;
        }

        if (file.Status != AudioFile::StatusType::READY)
        {
            engine.GetCommandQueue().AddCommand(std::make_shared<FunctionCommand>(
                    [this, &engine](Engine& e) -> CommandResult {
                        const AudioFile& f = GetAudioFile(AudioFile::DEFAULT);
                        if (f.Status == AudioFile::StatusType::FAILED)
                        {
                            log->Error("Trying to play a music theme that has failed to load: {0}", f.Filename);
                            return CommandResult::DONE;
                        }
                        if (f.Status == AudioFile::StatusType::READY || (m_MidiFiles.contains(""_hs) && m_MidiFiles[""_hs]->GetStatus() == MidiFile::StatusType::PARSING))
                        {
                            engine.GetCommandQueue().AddCommand(std::make_shared<ScheduleThemeChangeCommand>(m_Name));
                            return CommandResult::DONE;
                        }
                        return CommandResult::DEFER;
                    }));
            return false;
        }

        return true;
    }

    const AudioEffects& MusicTheme::GetAudioEffects(HashString type) const
    {
        if (m_AudioFiles.contains(type)) { return m_AudioEffects.at(type); }
        return AudioEffects::None;
    }

    std::shared_ptr<MidiFile> MusicTheme::GetMidiFile(HashString type) const
    {
        if (m_MidiFiles.contains(type)) { return m_MidiFiles.at(type); }
        return {};
    }

    bool MusicTheme::HasZone(HashString zone) const
    {
        zone = String(zone.GetValue()).MakeUpper();
        return std::find(m_Zones.begin(), m_Zones.end(), zone) != m_Zones.end();
    }

    std::shared_ptr<IChannel> MusicTheme::GetAcquiredChannel()
    {
        for (auto& channel: m_AcquiredChannels)
        {
            return channel;
        }
        return {};
    }

    void MusicTheme::ReleaseChannels()
    {
        for (auto& channel: m_AcquiredChannels)
        {
            channel->Release();
        }
        m_AcquiredChannels.clear();
    }

    String MusicTheme::ToString() const
    {
        String result = String("MusicTheme{ \n\tName: ") + m_Name + ", \n\tAudioFiles: {\n";
        int i = 0;
        for (auto& [type, audioFile]: m_AudioFiles)
        {
            result += String("\t\t") + String(type) + ": " + audioFile.ToString().Replace("\n", "\n\tt");
            if (++i < m_AudioFiles.size())
            {
                result += ",\n";
            }
        }
        i = 0;
        result += "\n\t},\n\tAudioEffects: { \n";
        for (auto& [type, audioEffects]: m_AudioEffects)
        {
            result += String("\t\t") + String(type) + ": " + audioEffects.ToString().Replace("\n", "\n\t\t");
            if (++i < m_AudioEffects.size())
            {
                result += ",\n";
            }
        }
        i = 0;
        result += "\n\t},\n\tZones: { ";
        for (auto& zone: m_Zones)
        {
            result += String(zone);
            if (++i < m_Zones.size())
            {
                result += ", ";
            }
        }
        result += " }\n }";
        return result;
    }
}