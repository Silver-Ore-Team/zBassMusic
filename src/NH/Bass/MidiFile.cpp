#include "MidiFile.h"

#include <Union/VDFS.h>
#include <unordered_map>
#include <vector>

namespace NH::Bass
{
    Logger* MidiFile::log = CreateLogger("zBassMusic::MidiFile");

    void MidiFile::LoadMidiFile(Executor& executor, const std::function<void(MidiFile&)>& onReady)
    {
        if (m_LoadingStatus != LoadingStatusType::NOT_LOADED)
        {
            log->Warning("MidiFile::LoadMidiFile() called on a MidiFile that is not in NOT_LOADED state.");
            return;
        }
        m_LoadingStatus = LoadingStatusType::LOADING;
        executor.AddTask([this, onReady]()
                         {
                             int systems = VDF_VIRTUAL | VDF_PHYSICAL;
                             const Union::VDFS::File* file = Union::VDFS::GetDefaultInstance().GetFile(m_Filename, systems);
                             if (!file)
                             {
                                 m_LoadingStatus = LoadingStatusType::FAILED;
                                 m_Error = "File not found";
                                 return;
                             }

                             Union::Stream* stream = file->Open();
                             m_Buffer.resize(stream->GetSize());
                             stream->Read(m_Buffer.data(), m_Buffer.size());
                             stream->Close();
                             m_LoadingStatus = LoadingStatusType::READY;
                             ParseMidiFile();

                             if (onReady)
                             {
                                onReady(*this);
                             }
                         });
    }

    void MidiFile::ParseMidiFile()
    {
        m_Status = StatusType::PARSING;
        m_Tones.clear();
        m_MidiEvents.clear();
        HSTREAM stream = BASS_MIDI_StreamCreateFile(true, m_Buffer.data(), 0, m_Buffer.size(), BASS_SAMPLE_FLOAT, 0);
        uint32_t count = BASS_MIDI_StreamGetEvents(stream, -1, 0, nullptr);
        m_MidiEvents.resize(count);
        BASS_MIDI_StreamGetEvents(stream, -1, 0, m_MidiEvents.data());

        uint32_t tempo = 0;
        float ppqn = 0.0f; // Ticks per quarter note
        BASS_ChannelGetAttribute(stream, BASS_ATTRIB_MIDI_PPQN, &ppqn);
        std::unordered_map<uint8_t, Tone> tonesInProgress;
        double ticksToSeconds = 0.0;

        for (const auto& event: m_MidiEvents)
        {
            switch (event.event)
            {
            case MIDI_EVENT_TEMPO:
                tempo = event.param;
                ticksToSeconds = (double) tempo / (double) ppqn / 1000000.0;
                log->Trace("TempoEvent: tempo={0}, ticksToSeconds={1}", tempo, ticksToSeconds);
                break;
            case MIDI_EVENT_NOTE:
            {
                uint8_t key = event.param & 0xFF;
                uint8_t velocity = (event.param >> 8) & 0xFF;
                uint32_t tick = event.tick;
                double time = tick * ticksToSeconds;
                log->Trace("NoteEvent: key={0} action={1} time={2}", key, velocity == 0 ? "release" : (velocity == 255 ? "stop" : "press"), time);
                if (velocity == 0 || velocity == 255)
                {
                    // release || stop
                    if (!tonesInProgress.contains(key))
                    {
                        log->Error("Error: key {0} released without being pressed at {1}", key, time);
                        continue;
                    }
                    Tone tone = tonesInProgress[key];
                    tone.end = time;
                    m_Tones.emplace_back(tone);
                    tonesInProgress.erase(key);
                }
                else
                {
                    // press
                    tonesInProgress[key] = { key, time, 0};
                }
                break;
            }
            }
        }

        for (const auto& tone : m_Tones)
        {
            log->Debug("Tone{ key={0}, start={1}, end={2} }", static_cast<uint32_t>(tone.key), tone.start, tone.end);
        }

        m_Status = StatusType::READY;
    }
}