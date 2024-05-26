#pragma once

#include "CommonTypes.h"
#include <NH/Logger.h>
#include <NH/HashString.h>
#include <NH/Executor.h>
#include <NH/ToString.h>
#include <NH/Bass/MidiFile.h>

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
        std::unordered_map<HashString, AudioFile> m_AudioFiles;
        std::unordered_map<HashString, AudioEffects> m_AudioEffects;
        std::unordered_map<HashString, std::shared_ptr<MidiFile>> m_MidiFiles;
        std::vector<HashString> m_Zones;

    public:
        static MusicTheme None;

        explicit MusicTheme(const String& name);

        void SetAudioFile(HashString type, const String& filename);

        void SetAudioEffects(HashString type, const std::function<void(AudioEffects&)>& effectsSetter);

        void AddZone(HashString zone);

        void AddMidiFile(HashString type, const std::shared_ptr<MidiFile>& midiFile);

        void LoadAudioFiles(Executor& executor);

        [[nodiscard]] const String& GetName() const { return m_Name; }

        [[nodiscard]] bool HasAudioFile(HashString type) const { return m_AudioFiles.find(type) != m_AudioFiles.end(); }

        [[nodiscard]] bool IsAudioFileReady(HashString type) const { return HasAudioFile(type) && m_AudioFiles.at(type).Status == AudioFile::StatusType::READY; }

        [[nodiscard]] const AudioFile& GetAudioFile(HashString type) const { return m_AudioFiles.at(type); }

        [[nodiscard]] const AudioEffects& GetAudioEffects(HashString type) const;

        [[nodiscard]] const std::shared_ptr<MidiFile>& GetMidiFile(HashString type) const;

        [[nodiscard]] const std::vector<HashString>& GetZones() const { return m_Zones; }

        [[nodiscard]] bool HasZone(HashString zone) const;

        [[nodiscard]] String ToString() const override
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

    private:
    };
}
