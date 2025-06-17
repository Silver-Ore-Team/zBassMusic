#include "MusicTheme.h"

#include "EngineCommands.h"
#include "NH/Bass/Command.h"
#include "NH/Bass/EventManager.h"

#include <Union/VDFS.h>

#include <utility>

namespace NH::Bass
{
    std::string AudioFile::DEFAULT = "DEFAULT";
    AudioEffects AudioEffects::None = {};
    MusicTheme MusicTheme::None = MusicTheme("<None>");
    Logger* MusicTheme::log = CreateLogger("zBassMusic::MusicTheme");

    MusicTheme::MusicTheme(std::string name) : m_Name(std::move(name)), m_TransitionInfo{std::string(m_Name)} {}

    void MusicTheme::SetAudioFile(const std::string& type, const std::string& filename)
    {
        if (type.empty()) {
            log->Error("MusicTheme::SetAudio(type, filename): error in {0}.", m_Name.c_str());
            log->Error("MusicTheme::SetAudio(type, filename): type is empty or invalid string. Skipping.");
            if (!filename.empty()) {
                log->Error("MusicTheme::SetAudio(type, filename): filename was not empty and is equal to {0}.", filename.c_str());
            }
            return;
        }

        if (filename.empty()) {
            log->Error("MusicTheme::SetAudio(type, filename): error in {0}.", m_Name.c_str());
            log->Error("MusicTheme::SetAudio(type, filename): filename is empty or invalid string. Skipping.");
            if (!type.empty()) {
                log->Error("MusicTheme::SetAudio(type, filename): type was not empty and is equal to {0}.", filename.c_str());
            }
            return;
        }

        if (!m_AudioFiles.contains(type)) {
            log->Info("MusicTheme::SetAudioFile(type, filename): Emplacing new audio file {0} to {1}.", type.c_str(), filename.c_str());
            m_AudioFiles.emplace(std::string(type), AudioFile{});
        }
        else {
            log->Info("MusicTheme::SetAudioFile(type, filename): File audio object already exists {0} to {1}, skipping emplacement.", type.c_str(), filename.c_str());
        }

        log->Info("MusicTheme::SetAudioFile(type, filename): Setting .Filename to {0}.", filename.c_str());
        m_AudioFiles[type].Filename = filename;
        log->Info("MusicTheme::SetAudioFile(type, filename): Setting .Status to AudioFile::StatusType::NOT_LOADED.");
        m_AudioFiles[type].Status = AudioFile::StatusType::NOT_LOADED;
    }

    void MusicTheme::SetAudioEffects(const std::string& type, const std::function<void(AudioEffects&)>& effectsSetter)
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

    void MusicTheme::AddZone(const std::string& zone)
    {
        m_Zones.emplace_back(zone);
    }

    void MusicTheme::AddMidiFile(const std::string& type, const std::shared_ptr<MidiFile>& midiFile)
    {
        m_MidiFiles.emplace(std::make_pair(std::string(type), midiFile));
        m_MidiFiles[type]->LoadMidiFile(Executors.IO, [this, type](const MidiFile& midi) {
            m_TransitionInfo.AddMidiFile(midi, type);
        });
    }

    void MusicTheme::AddJingle(const std::string& filename, const double delay, const std::string& filter)
    {
        const std::string key = "Jingle_" + filter;
        SetAudioFile(key, filename);
        LoadAudioFiles(Executors.IO);
        AudioFile& jingle = m_AudioFiles.at(key);
        m_TransitionInfo.AddJingle(std::shared_ptr<AudioFile>(&jingle), delay, filter);
        log->Info("New jingle created {0}: {1}", filter.c_str(), filename.c_str());
    }

    void MusicTheme::LoadAudioFiles(Executor& executor)
    {
        for (auto& [type, audioFile]: m_AudioFiles)
        {
            if (audioFile.Status == AudioFile::StatusType::NOT_LOADED)
            {
                audioFile.Status = AudioFile::StatusType::LOADING;
                executor.AddTask([type, this] {
                    int systems = VDF_VIRTUAL | VDF_PHYSICAL;
                    const Union::VDFS::File* file = Union::VDFS::GetDefaultInstance().GetFile(m_AudioFiles[type].Filename.c_str(), systems);
                    if (!file)
                    {
                        m_AudioFiles[type].Status = AudioFile::StatusType::FAILED;
                        m_AudioFiles[type].Error = "File not found";
                        return;
                    }

                    auto* stream = file->Open();
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
        if (nextTheme.GetName() == GetName())
        {
            return;
        }

        const auto& transition = m_TransitionInfo.GetTransition(nextTheme.GetName());
        log->Debug("Transition {0} to {1}", GetName().c_str(), nextTheme.GetName().c_str());
        log->PrintRaw(LoggerLevel::Trace, transition.ToString().c_str());

        if (!HasAudioFile(AudioFile::DEFAULT))
        {
            log->Error("{0} needs to transition into {1} but it doesn't have a DEFAULT audio. Switching to {1} without transitions",
                       GetName().c_str(), nextTheme.GetName().c_str());
            return;
        }

        if (!ReadyToPlay(engine))
        {
            if (const auto& file = GetAudioFile(AudioFile::DEFAULT); file.Status == AudioFile::StatusType::FAILED)
            {
                log->Warning("{0} needs to transition into {1} but the audio file failed to load. Switching to {1} without transitions",
                             GetName().c_str(), nextTheme.GetName().c_str());
                engine.GetCommandQueue().AddCommand(std::make_shared<PlayThemeInstantCommand>(nextTheme.GetName()));
                return;
            }
        }

        auto channel = GetAcquiredChannel();
        if (!channel) channel = m_AcquiredChannels.emplace_back(engine.AcquireFreeChannel());
        std::optional<Transition::TimePoint> timePoint = transition.NextAvailableTimePoint(channel->Position());

        const std::function<void()>& playJingle = CreateSyncHandler([&engine, &transition, this] {
            if (transition.Jingle)
            {
                auto jingleChannel = engine.AcquireFreeChannel();
                jingleChannel->SetLoop(false);
                if (auto result = jingleChannel->PlayInstant(*transition.Jingle, AudioEffects::None)) {
                    jingleChannel->OnAudioEnds(CreateSyncHandler([jingleChannel]() { jingleChannel->Release(); }));
                }
                else
                {
                    log->Error("Could not play jingle: {0}", result.error().what());
                    jingleChannel->Release();
                }
            }
        });

        if (timePoint && channel->IsPlaying())
        {
            log->Trace("OnPosition: {0}", String(timePoint->Start));
            channel->OnPosition(timePoint->Start, CreateSyncHandler([&engine, &transition, this] {
                                    Stop(engine, transition);
                                }));
            engine.GetCommandQueue().AddCommand(std::make_shared<OnTimeCommand>(
                    std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<int64_t>((timePoint->Start + transition.JingleDelay) * 1000)),
                    std::make_shared<FunctionCommand>([&playJingle]([[maybe_unused]] Engine& jingleEngine) -> CommandResult {
                        playJingle();
                        return CommandResult::DONE;
                    })));
            channel->OnPosition(timePoint->NextStart, CreateSyncHandler([&engine, &nextTheme, &transition, timePoint] {
                                    nextTheme.Play(engine, transition, timePoint);
                                }));
        }
        else
        {
            Stop(engine, transition);
            engine.GetCommandQueue().AddCommand(std::make_shared<OnTimeCommand>(
                    std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<int64_t>(transition.JingleDelay * 1000)),
                    std::make_shared<FunctionCommand>([&playJingle]([[maybe_unused]] Engine& jingleEngine) -> CommandResult {
                        playJingle();
                        return CommandResult::DONE;
                    })));
            nextTheme.Play(engine, transition);
        }
    }

    void MusicTheme::Play(IEngine& engine) { Play(engine, Transition::EMPTY); }
    void MusicTheme::Play(IEngine& engine, const struct Transition& transition, const std::optional<Transition::TimePoint>& timePoint)
    {
        if (!HasAudioFile(AudioFile::DEFAULT))
        {
            log->Error("{0} can't play because it doesn't have DEFAULT audio file", GetName().c_str());
            return;
        }
        if (!ReadyToPlay(engine)) {
            return;
        }

        const auto channel = m_AcquiredChannels.emplace_back(engine.AcquireFreeChannel());
        auto& effects = GetAudioEffects(AudioFile::DEFAULT);
        const auto effect = timePoint.has_value() ? timePoint.value().NextEffect : transition.Effect;
        if (const auto result = channel->PlayInstant(GetAudioFile(AudioFile::DEFAULT), effects); !result)
        {
            ReleaseChannels();
            return;
        }

        channel->SetLoop(effects.Loop.Active);

        if (effects.Loop.Active)
        {
            if (effects.Loop.Start && effects.Loop.End > 0)
            {
                channel->OnLoopEnd(effects.Loop.Start, effects.Loop.End, CreateSyncHandler([]{}));
            }
        }

        if (effects.ReverbDX8.Active)
        {
            channel->SetDX8ReverbEffect(effects.ReverbDX8.Mix, effects.ReverbDX8.Time, 0, 0.001f);
        }

        if (effect == TransitionEffect::CROSSFADE)
        {
            channel->SetVolume(0.0f);
            channel->SlideVolume(1.0f, timePoint.has_value() ? static_cast<uint32_t>(timePoint.value().Duration) * 1000 : static_cast<uint32_t>(effects.FadeIn.Duration));
        }
        else
        {
            channel->SetVolume(effects.Volume.Active ? effects.Volume.Volume : 1.0f);
        }

        if (effects.FadeOut.Active)
        {
            channel->BeforeAudioEnds(effects.FadeOut.Duration, CreateSyncHandler([this, &engine](const double timeLeft) -> void {
                                         engine.GetEM().DispatchEvent(MusicTransitionEvent{this, AudioFile::DEFAULT, static_cast<float>(timeLeft)});
                                     }));
        }

        channel->OnAudioEnds(CreateSyncHandler([this, &engine]() -> void { engine.GetEM().DispatchEvent(MusicEndEvent{this, AudioFile::DEFAULT}); }));
        engine.GetEM().DispatchEvent(MusicChangeEvent{this, AudioFile::DEFAULT});
    }

    void MusicTheme::Stop(IEngine& engine) { Stop(engine, m_TransitionInfo.GetDefaultTransition()); }
    void MusicTheme::Stop([[maybe_unused]] IEngine& engine, const struct Transition& transition)
    {
        auto channel = GetAcquiredChannel();
        if (!channel)
        {
            log->Warning("Requested to stop {0} but it does not have any playing channels.", m_Name.c_str());
            ReleaseChannels();
            return;
        };
        std::optional<Transition::TimePoint> timePoint = transition.NextAvailableTimePoint(channel->Position());
        if (const auto effect = timePoint.has_value() ? timePoint.value().Effect : transition.Effect; effect == TransitionEffect::CROSSFADE)
        {
            const uint32_t time = timePoint.has_value() ? static_cast<uint32_t>(timePoint.value().Duration) * 1000 : static_cast<uint32_t>(transition.EffectDuration);
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

    bool MusicTheme::ReadyToPlay(IEngine& engine)
    {
        if (!HasAudioFile(AudioFile::DEFAULT))
        {
            log->Error("{0} can't play because it doesn't have DEFAULT audio file", GetName().c_str());
            return false;
        }

        const auto& file = GetAudioFile(AudioFile::DEFAULT);
        if (file.Status == AudioFile::StatusType::FAILED)
        {
            log->Error("Trying to play a music theme that has failed to load: {0}", file.Filename.c_str());
            return false;
        }

        if (file.Status != AudioFile::StatusType::READY)
        {
            engine.GetCommandQueue().AddCommand(std::make_shared<FunctionCommand>(
                    [this, &engine]([[maybe_unused]] Engine& e) -> CommandResult {
                        const AudioFile& f = GetAudioFile(AudioFile::DEFAULT);
                        if (f.Status == AudioFile::StatusType::FAILED)
                        {
                            log->Error("Trying to play a music theme that has failed to load: {0}", f.Filename.c_str());
                            return CommandResult::DONE;
                        }
                        if (f.Status == AudioFile::StatusType::READY || (m_MidiFiles.contains("") && m_MidiFiles[""]->GetStatus() == MidiFile::StatusType::PARSING))
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

    const AudioEffects& MusicTheme::GetAudioEffects(const std::string& type) const
    {
        if (m_AudioFiles.contains(type)) {
            return m_AudioEffects.at(type);
        }
        return AudioEffects::None;
    }

    std::shared_ptr<MidiFile> MusicTheme::GetMidiFile(const std::string& type) const
    {
        if (m_MidiFiles.contains(type)) {
            return m_MidiFiles.at(type);
        }
        return {};
    }

    bool MusicTheme::HasZone(std::string zone) const
    {
        zone = String(zone.c_str()).MakeUpper();
        return std::ranges::find(m_Zones, zone) != m_Zones.end();
    }

    std::shared_ptr<IChannel> MusicTheme::GetAcquiredChannel() const
    {
        if (m_AcquiredChannels.size() > 0)
        {
            return m_AcquiredChannels[0];
        }
        return {};
    }

    void MusicTheme::ReleaseChannels()
    {
        for (const auto& channel: m_AcquiredChannels)
        {
            channel->Release();
        }
        m_AcquiredChannels.clear();
    }

    std::string MusicTheme::ToString() const
    {
        std::string result = "MusicTheme{ \n\tName: " + m_Name + ", \n\tAudioFiles: {\n";
        size_t i = 0;
        for (auto& [type, audioFile]: m_AudioFiles)
        {
            result += "\t\t" + type + ": " + String(audioFile.ToString().c_str()).Replace("\n", "\n\tt").ToChar();
            if (++i < m_AudioFiles.size())
            {
                result += ",\n";
            }
        }
        i = 0;
        result += "\n\t},\n\tAudioEffects: { \n";
        for (auto& [type, audioEffects]: m_AudioEffects)
        {
            result += "\t\t" + type + ": " + String(audioEffects.ToString().c_str()).Replace("\n", "\n\t\t").ToChar();
            if (++i < m_AudioEffects.size())
            {
                result += ",\n";
            }
        }
        i = 0;
        result += "\n\t},\n\tZones: { ";
        for (const auto& zone: m_Zones)
        {
            result += zone;
            if (++i < m_Zones.size())
            {
                result += ", ";
            }
        }
        result += " }\n }";
        return result;
    }
}// namespace NH::Bass