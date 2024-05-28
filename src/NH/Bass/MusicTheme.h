#pragma once

#include <NH/Logger.h>
#include <NH/HashString.h>
#include <NH/Executor.h>
#include <NH/ToString.h>
#include <NH/Bass/MidiFile.h>
#include <NH/Bass/IEngine.h>
#include <NH/Bass/IChannel.h>
#include <NH/Bass/TransitionInfo.h>

#include <vector>
#include <unordered_map>
#include <functional>

namespace NH::Bass
{
    struct AudioFile : public HasToString
    {
        static HashString DEFAULT;

        enum class StatusType : size_t { NOT_LOADED = 0, LOADING, READY, FAILED };

        String Filename;
        std::vector<char> Buffer = {};
        StatusType Status = StatusType::NOT_LOADED;
        String Error;

        [[nodiscard]] String ToString() const override
        {
            static String types[] = { "NOT_LOADED", "LOADING", "READY", "FAILED" };
            return String("AudioFile{ Filename: ") + Filename + ", Status: " + types[(size_t) Status] + ", Error: " + Error + " }";
        }
    };

    struct AudioEffects : public HasToString
    {
        static AudioEffects None;

        struct { bool Active = false; float Mix = 0; float Time = 0;  } ReverbDX8{};
        struct { bool Active = false; double Duration = 0; } FadeIn{};
        struct { bool Active = false; double Duration = 0; } FadeOut{};
        struct { bool Active = false; float Volume = 1.0f; } Volume{};
        struct { bool Active = false; } Loop{};

        [[nodiscard]] String ToString() const override
        {
            return String("AudioEffects{ \n"
                          "\tReverbDX8: { Active: ") + String(ReverbDX8.Active) + ", Mix: " + String(ReverbDX8.Mix) + ", Time: " + String(ReverbDX8.Time) + " }, \n"
                   + "\tFadeIn: { Active: " + String(FadeIn.Active) + ", Duration: " + String(FadeIn.Duration) + " }, \n"
                   + "\tFadeOut: { Active: " + String(FadeOut.Active) + ", Duration: " + String(FadeOut.Duration) + " }, \n"
                   + "\tVolume: { Active: " + String(Volume.Active) + ", Volume: " + String(Volume.Volume) + " } \n}";
        }
    };

    class MusicTheme : public HasToString
    {
        static Logger* log;

        String m_Name;
        size_t m_SyncHandlersId = 0;
        TransitionInfo m_TransitionInfo;
        std::unordered_map<HashString, AudioFile> m_AudioFiles;
        std::unordered_map<HashString, AudioEffects> m_AudioEffects;
        std::unordered_map<HashString, std::shared_ptr<MidiFile>> m_MidiFiles;
        std::unordered_map<size_t, std::function<void()>> m_SyncHandlers;
        std::unordered_map<size_t, std::function<void(double)>> m_SyncHandlersWithDouble;
        std::vector<HashString> m_Zones;
        std::vector<std::shared_ptr<IChannel>> m_AcquiredChannels;

    public:
        static MusicTheme None;

        explicit MusicTheme(const String& name);

        void SetAudioFile(HashString type, const String& filename);
        void SetAudioEffects(HashString type, const std::function<void(AudioEffects&)>& effectsSetter);
        void AddZone(HashString zone);
        void AddMidiFile(HashString type, std::shared_ptr<MidiFile> midiFile);
        void AddJingle(const String& filename, double delay, HashString filter);
        void LoadAudioFiles(Executor& executor);

        void Schedule(IEngine& engine, const std::shared_ptr<MusicTheme>& currentTheme);
        void Transition(IEngine& engine, MusicTheme& nextTheme);
        void Play(IEngine& engine);
        void Play(IEngine& engine, const struct Transition& transition, std::optional<Transition::TimePoint> timePoint = std::nullopt);
        void Stop(IEngine& engine);
        void Stop(IEngine& engine, const struct Transition& transition);

        [[nodiscard]] const String& GetName() const { return m_Name; }
        [[nodiscard]] TransitionInfo& GetTransitionInfo() { return m_TransitionInfo; };
        [[nodiscard]] bool HasAudioFile(HashString type) const { return m_AudioFiles.find(type) != m_AudioFiles.end(); }
        [[nodiscard]] bool IsAudioFileReady(HashString type) const { return HasAudioFile(type) && m_AudioFiles.at(type).Status == AudioFile::StatusType::READY; }
        [[nodiscard]] const AudioFile& GetAudioFile(HashString type) const { return m_AudioFiles.at(type); }
        [[nodiscard]] const AudioEffects& GetAudioEffects(HashString type) const;
        [[nodiscard]] std::shared_ptr<MidiFile> GetMidiFile(HashString type) const;
        [[nodiscard]] const std::vector<HashString>& GetZones() const { return m_Zones; }
        [[nodiscard]] bool HasZone(HashString zone) const;
        [[nodiscard]] String ToString() const override;

    private:
        bool ReadyToPlay(IEngine& engine, HashString audio);
        std::shared_ptr<IChannel> GetAcquiredChannel();
        void ReleaseChannels();

        const std::function<void(double)>& CreateSyncHandler(std::function<void(double)> function)
        {
            size_t id = m_SyncHandlersId++;
            log->Trace("SyncHandler id: {0}", id);
            auto handler = [this, function = std::move(function), id](double value) {
                function(value);
                m_SyncHandlersWithDouble.erase(id);
            };
            m_SyncHandlersWithDouble.emplace(id, std::move(handler));
            return m_SyncHandlersWithDouble.at(id);
        }

        const std::function<void()>& CreateSyncHandler(std::function<void()> function)
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
