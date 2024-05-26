#pragma once

#include <NH/Logger.h>
#include <NH/HashString.h>
#include <NH/Executor.h>

#include <bass.h>
#include <bassmidi.h>

namespace NH::Bass
{
    /**
     * @brief Represents a MIDI file with metadata for the music transitions.
     *
     * The MIDI file is parser and the notes are extracted and stored in the `m_Tones` vector.
     * Specific note keys mean:
     *  - A4 (key=69): Fade out transition
     *      Fade-out starts at the START time and decreases the volume to 0 at the END time.
     *      By default, the next song is fading-in for the same period.
     *  - A#4 (key=70): Fade in transition for next song (optional)
     *      If present alongside A4, defines the fade-in period of the next song.
     */
    class MidiFile
    {
    public:
        struct Tone {
            uint32_t key;
            double start;
            double end;
        };

        enum class StatusType : size_t { NOT_LOADED = 0, PARSING, READY, FAILED };
        enum class LoadingStatusType : size_t { NOT_LOADED = 0, LOADING, READY, FAILED };

    private:
        static Logger* log;
        HashString m_ThemeId;
        HashString m_AudioId;
        String m_Filename;
        std::vector<char> m_Buffer{};
        StatusType m_Status = StatusType::NOT_LOADED;
        LoadingStatusType m_LoadingStatus = LoadingStatusType::NOT_LOADED;
        String m_Error{};
        std::vector<BASS_MIDI_EVENT> m_MidiEvents{};
        std::vector<Tone> m_Tones{};

    public:
        MidiFile(HashString themeId, HashString audioId, const String& filename)
                : m_ThemeId(themeId), m_AudioId(audioId), m_Filename(filename) {}

        void LoadMidiFile(Executor& executor);

        [[nodiscard]] HashString GetThemeId() const { return m_ThemeId; }

        [[nodiscard]] HashString GetAudioId() const { return m_AudioId; }

        [[nodiscard]] const String& GetFilename() const { return m_Filename; }

        [[nodiscard]] StatusType GetStatus() const { return m_Status; }

        [[nodiscard]] LoadingStatusType GetLoadingStatus() const { return m_LoadingStatus; }

        [[nodiscard]] const String& GetError() const { return m_Error; }

        [[nodiscard]] const std::vector<Tone>& GetTones() const { return m_Tones; }

    private:
        void ParseMidiFile();
    };
}
