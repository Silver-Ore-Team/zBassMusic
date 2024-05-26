#include "MusicTheme.h"

#include <Union/VDFS.h>

namespace NH::Bass
{
    HashString AudioFile::DEFAULT = "DEFAULT";
    AudioEffects AudioEffects::None = {};
    MusicTheme MusicTheme::None = MusicTheme("<None>");
    Logger* MusicTheme::log = CreateLogger("zBassMusic::MusicTheme");

    MusicTheme::MusicTheme(const String& name)
            : m_Name(name)
    {

    }

    void MusicTheme::SetAudioFile(HashString type, const String& filename)
    {
        m_AudioFiles.emplace(std::make_pair(type, AudioFile{}));
        m_AudioFiles[type].Filename = filename;
        m_AudioFiles[type].Status = AudioFile::StatusType::NOT_LOADED;
    }

    void MusicTheme::SetAudioEffects(HashString type, const std::function<void(AudioEffects&)>& effectsSetter)
    {
        m_AudioEffects.emplace(std::make_pair(type, AudioEffects{}));
        effectsSetter(m_AudioEffects[type]);
    }

    void MusicTheme::AddZone(HashString zone)
    {
        m_Zones.emplace_back(zone);
    }

    void MusicTheme::AddMidiFile(HashString type, const std::shared_ptr<MidiFile>& midiFile)
    {
        m_MidiFiles.emplace(std::make_pair(type, midiFile));
    }

    void MusicTheme::LoadAudioFiles(Executor& executor)
    {
        for (auto& [type, audioFile]: m_AudioFiles)
        {
            if (audioFile.Status == AudioFile::StatusType::NOT_LOADED)
            {
                audioFile.Status = AudioFile::StatusType::LOADING;
                executor.AddTask([type, this]()
                                 {
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

    const AudioEffects& MusicTheme::GetAudioEffects(HashString type) const
    {
        if (m_AudioFiles.contains(type)) { return m_AudioEffects.at(type); }
        return AudioEffects::None;
    }

    const std::shared_ptr<MidiFile>& MusicTheme::GetMidiFile(HashString type) const
    {
        if (m_MidiFiles.contains(type)) { return m_MidiFiles.at(type); }
        return {};
    }

    bool MusicTheme::HasZone(HashString zone) const
    {
        return std::find(m_Zones.begin(), m_Zones.end(), zone) != m_Zones.end();
    }
}