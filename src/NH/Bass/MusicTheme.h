#pragma once

#include "NH/Logger.h"
#include "NH/Executor.h"
#include "NH/ToString.h"
#include "NH/Bass/MidiFile.h"
#include "NH/Bass/IEngine.h"
#include "NH/Bass/IChannel.h"
#include "NH/Bass/TransitionInfo.h"

#include <vector>
#include <unordered_map>
#include <functional>

namespace NH::Bass
{
    class AudioFile final : public HasToString
    {
    public:
        static std::string DEFAULT;

        enum class StatusType : size_t { NOT_LOADED = 0, LOADING, READY, FAILED };

        std::string Filename;
        std::vector<char> Buffer = {};
        StatusType Status = StatusType::NOT_LOADED;
        std::string Error;

        [[nodiscard]] std::string ToString() const override
        {
            static const std::string types[] = { "NOT_LOADED", "LOADING", "READY", "FAILED" };
            return std::string("AudioFile{ Filename: ") + Filename + ", Status: " + types[static_cast<size_t>(Status)] + ", Error: " + Error + " }";
        }
    };

    struct AudioEffects final : HasToString
    {
        static AudioEffects None;

        struct { bool Active = false; float Mix = 0; float Time = 0;  } ReverbDX8{};
        struct { bool Active = false; double Duration = 0; } FadeIn{};
        struct { bool Active = false; double Duration = 0; } FadeOut{};
        struct { bool Active = false; float Volume = 1.0f; } Volume{};
        struct { bool Active = false; } Loop{};

        [[nodiscard]] std::string ToString() const override
        {
            return std::string("AudioEffects{ \n"
                          "\tReverbDX8: { Active: ") + std::string(ReverbDX8.Active ? "yes" : "no") + ", Mix: " + std::string(ReverbDX8.Mix ? "yes" : "no") + ", Time: " + std::string(ReverbDX8.Time ? "yes" : "no") + " }, \n"
                   + "\tFadeIn: { Active: " + std::string(FadeIn.Active ? "yes" : "no") + ", Duration: " + std::string(FadeIn.Duration ? "yes" : "no") + " }, \n"
                   + "\tFadeOut: { Active: " + std::string(FadeOut.Active ? "yes" : "no") + ", Duration: " + std::string(FadeOut.Duration ? "yes" : "no") + " }, \n"
                   + "\tVolume: { Active: " + std::string(Volume.Active ? "yes" : "no") + ", Volume: " + std::string(Volume.Volume ? "yes" : "no") + " } \n}";
        }
    };

    class MusicTheme final : public HasToString
    {
        static Logger* log;

        std::string m_Name;
        size_t m_SyncHandlersId = 0;
        TransitionInfo m_TransitionInfo;
        std::unordered_map<std::string, AudioFile> m_AudioFiles;
        std::unordered_map<std::string, AudioEffects> m_AudioEffects;
        std::unordered_map<std::string, std::shared_ptr<MidiFile>> m_MidiFiles;
        std::unordered_map<size_t, std::function<void()>> m_SyncHandlers;
        std::unordered_map<size_t, std::function<void(double)>> m_SyncHandlersWithDouble;
        std::unordered_map<size_t, std::function<void(bool)>> m_SyncHandlersWithBoolean;
        std::vector<std::string> m_Zones;
        std::vector<std::shared_ptr<IChannel>> m_AcquiredChannels;

    public:
        static MusicTheme None;

        explicit MusicTheme(std::string  name);

        void SetAudioFile(const std::string& type, const std::string& filename);
        void SetAudioEffects(const std::string& type, const std::function<void(AudioEffects&)>& effectsSetter);
        void AddZone(const std::string& zone);
        void AddMidiFile(const std::string& type, const std::shared_ptr<MidiFile>& midiFile);
        void AddJingle(const std::string& filename, double delay, const std::string& filter);
        void LoadAudioFiles(Executor& executor);

        void Schedule(IEngine& engine, const std::shared_ptr<MusicTheme>& currentTheme);
        void Transition(IEngine& engine, MusicTheme& nextTheme);
        void Play(IEngine& engine);
        void Play(IEngine& engine, const struct Transition& transition, const std::optional<Transition::TimePoint>& timePoint = std::nullopt);
        void Stop(IEngine& engine);
        void Stop(IEngine& engine, const struct Transition& transition);

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] TransitionInfo& GetTransitionInfo() { return m_TransitionInfo; };
        [[nodiscard]] bool HasAudioFile(const std::string& type) const { return m_AudioFiles.contains(type); }
        [[nodiscard]] bool IsAudioFileReady(const std::string& type) const { return HasAudioFile(type) && m_AudioFiles.at(type).Status == AudioFile::StatusType::READY; }
        [[nodiscard]] const AudioFile& GetAudioFile(const std::string& type) const { return m_AudioFiles.at(type); }
        [[nodiscard]] const AudioEffects& GetAudioEffects(const std::string& type) const;
        [[nodiscard]] std::shared_ptr<MidiFile> GetMidiFile(const std::string& type) const;
        [[nodiscard]] const std::vector<std::string>& GetZones() const { return m_Zones; }
        [[nodiscard]] bool HasZone(std::string zone) const;
        [[nodiscard]] std::string ToString() const override;

    private:
        bool ReadyToPlay(IEngine& engine);
        std::shared_ptr<IChannel> GetAcquiredChannel() const;
        void ReleaseChannels();

        const std::function<void(double)>& CreateSyncHandler(std::function<void(double)>&& function)
        {
            size_t id = m_SyncHandlersId++;
            log->Trace("SyncHandler id: {0}", id);
            auto handler = [this, function = std::move(function), id](const double value) {
                function(value);
                m_SyncHandlersWithDouble.erase(id);
            };
            m_SyncHandlersWithDouble.emplace(id, std::move(handler));
            return m_SyncHandlersWithDouble.at(id);
        }

        const std::function<void(bool)>& CreateSyncHandler(std::function<void(bool)>&& function)
        {
            size_t id = m_SyncHandlersId++;
            log->Trace("SyncHandler id: {0}", id);
            auto handler = [this, function = std::move(function), id](const bool value) {
                function(value);
                m_SyncHandlersWithBoolean.erase(id);
            };
            m_SyncHandlersWithBoolean.emplace(id, std::move(handler));
            return m_SyncHandlersWithBoolean.at(id);
        }

        const std::function<void()>& CreateSyncHandler(std::function<void()>&& function)
        {
            size_t id = m_SyncHandlersId++;
            log->Trace("SyncHandler id: {0}", id);
            auto handler = [this, function = std::move(function), id]() {
                function();
                m_SyncHandlers.erase(id);
            };
            m_SyncHandlers.emplace(id, std::move(handler));
            return m_SyncHandlers.at(id);
        }
    };
}
