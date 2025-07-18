#include "NH/Bass/MidiFile.h"

#include <functional>

namespace GOTHIC_NAMESPACE
{
    struct GothicMusicTheme {
        zSTRING fileName;
        float vol;
        bool loop;
        float reverbMix;
        float reverbTime;
        zTMus_TransType trType;
        zTMus_TransSubType trSubType;
    };

    enum class BassMusicThemeType : size_t
    {
        NORMAL = 0
    };

    struct BassMusicTheme {
        zSTRING Name;
        zSTRING Zones;
        BassMusicThemeType Type = BassMusicThemeType::NORMAL;
    };

    struct BassMusicThemeAudio {
        zSTRING Theme;
        zSTRING Type;
        zSTRING Filename;
        zSTRING MidiFile;
        float Volume;
        int Loop;
        float LoopStart;
        float LoopEnd;
        int Reverb;
        float ReverbMix;
        float ReverbTime;
        int FadeIn;
        int FadeInDuration;
        int FadeOut;
        int FadeOutDuration;
    };

    class BassLoader
    {
        zCParser* m_Parser;
        std::vector<GothicMusicTheme*> m_GothicThemeInstances;
        std::vector<BassMusicTheme*> m_BassThemeInstances;
        std::vector<BassMusicThemeAudio*> m_BassThemeAudioInstances;

    public:
        explicit BassLoader(zCParser* p) : m_Parser(p) {}

        ~BassLoader()
        {
            for (const auto theme: m_GothicThemeInstances) {
                delete theme;
            }
            for (const auto theme: m_BassThemeInstances) {
                delete theme;
            }
            for (const auto theme: m_BassThemeAudioInstances) {
                delete theme;
            }
        }

        void Load()
        {
            LoadGothic();
            LoadBass();

            const zSTRING initFuncName("BassMusic_Init");
            if (const zCPar_Symbol* symbol = m_Parser->GetSymbol(initFuncName); symbol && symbol->type == zPAR_TYPE_FUNC)
            {
                m_Parser->CallFunc(initFuncName);
            }
        }

    private:
        void LoadGothic()
        {
            ForEachClass<GothicMusicTheme>(
                    "C_MUSICTHEME",
                    [&]() { return m_GothicThemeInstances.emplace_back(new GothicMusicTheme{}); },
                    [&](const GothicMusicTheme* input, const zCPar_Symbol* symbol) {
                        const auto theme = std::make_shared<NH::Bass::MusicTheme>(symbol->name.ToChar());
                        theme->SetAudioFile(NH::Bass::AudioFile::DEFAULT, input->fileName.ToChar());
                        theme->SetAudioEffects(NH::Bass::AudioFile::DEFAULT, [&](NH::Bass::AudioEffects& effects) {
                            effects.Loop.Active = input->loop;
                            effects.Volume.Active = true;
                            effects.Volume.Volume = input->vol;
                            if (!NH::Bass::Options->ForceDisableReverb)
                            {
                                effects.ReverbDX8.Active = true;
                                effects.ReverbDX8.Mix = input->reverbMix;
                                effects.ReverbDX8.Time = input->reverbTime;
                            }
                            const bool forceFade = NH::Bass::Options->ForceFadeTransition;
                            if (forceFade || input->trType == zMUS_TR_INTRO || input->trType == zMUS_TR_ENDANDINTRO)
                            {
                                effects.FadeIn.Active = true;
                                effects.FadeIn.Duration = NH::Bass::Options->TransitionTime;
                            }
                            if (forceFade || input->trType == zMUS_TR_END || input->trType == zMUS_TR_ENDANDINTRO)
                            {
                                effects.FadeOut.Active = true;
                                effects.FadeOut.Duration = NH::Bass::Options->TransitionTime;
                            }
                        });
                        zSTRING zone = symbol->name;
                        zone.Upper();
                        theme->AddZone(zone.ToChar());
                        NH::Bass::Engine::GetInstance()->GetMusicManager().AddTheme(symbol->name.ToChar(), theme);
                    });
        }

        void LoadBass()
        {
            static NH::Logger* log = NH::CreateLogger("BassLoader::LoadBass");

            ForEachClass<BassMusicTheme>(
                    Globals->BassMusicThemeClassName,
                    [&] { return m_BassThemeInstances.emplace_back(new BassMusicTheme{}); },
                    [&](const BassMusicTheme* input, [[maybe_unused]] const zCPar_Symbol* symbol) {
                        const auto theme = std::make_shared<NH::Bass::MusicTheme>(input->Name.ToChar());
                        theme->SetAudioEffects(NH::Bass::AudioFile::DEFAULT, []([[maybe_unused]] const NH::Bass::AudioEffects& effects) {});
                        for (auto zones = NH::String(input->Zones.ToChar()).Split(","); auto& zone: zones)
                        {
                            theme->AddZone(std::string(zone.MakeUpper().ToChar()));
                        }
                        // input->Type ignored for now
                        NH::Bass::Engine::GetInstance()->GetMusicManager().AddTheme(input->Name.ToChar(), theme);
                    });

            ForEachClass<BassMusicThemeAudio>(
                    Globals->BassMusicThemeAudioClassName,
                    [&] { return m_BassThemeAudioInstances.emplace_back(new BassMusicThemeAudio{}); },
                    [&](const BassMusicThemeAudio* input, const zCPar_Symbol* symbol) {
                        const std::shared_ptr<NH::Bass::MusicTheme> theme = NH::Bass::Engine::GetInstance()->GetMusicManager().GetTheme(input->Theme.ToChar());
                        if (!theme) {
                            log->Error("Could not find theme '{0}' referenced by {1}(BassMusic_ThemeAudio). Skipping...", input->Theme.ToChar(), symbol->name.ToChar());
                            log->Error("Make sure that {0}.theme == BassMusic_Theme.name.", symbol->name.ToChar());
                            return;
                        }
                        auto type = NH::String(input->Type.ToChar());
                        const std::string id = type == "DEFAULT" ? NH::Bass::AudioFile::DEFAULT : type.ToChar();
                        theme->SetAudioFile(id, input->Filename.ToChar());
                        theme->SetAudioEffects(id, [&](NH::Bass::AudioEffects& effects) {
                            if (input->Loop)
                            {
                                effects.Loop.Active = true;
                                if (input->LoopStart >= 0 && input->LoopEnd > input->LoopStart)
                                {
                                    effects.Loop.Start = input->LoopStart;
                                    effects.Loop.End = input->LoopEnd;
                                }
                                else if (input->LoopStart == 0 && input->LoopEnd == 0)
                                {
                                    // actually do nothing because `effects.Loop.Active = true` is enough. 
                                }
                                else
                                {
                                    log->Error("Invalid loop range for {0} in {1}. Skipping...", id.c_str(), symbol->name.ToChar());
                                }
                            }
                            else
                            {
                                effects.Loop.Active = false;
                            }
                            effects.Volume.Active = true;
                            effects.Volume.Volume = input->Volume;
                            if (!NH::Bass::Options->ForceDisableReverb && input->Reverb)
                            {
                                effects.ReverbDX8.Active = true;
                                effects.ReverbDX8.Mix = input->ReverbMix;
                                effects.ReverbDX8.Time = input->ReverbTime;
                            }
                            bool forceFade = NH::Bass::Options->ForceFadeTransition;
                            if (forceFade || input->FadeIn)
                            {
                                effects.FadeIn.Active = true;
                                effects.FadeIn.Duration = input->FadeInDuration;
                            }
                            if (forceFade || input->FadeOut)
                            {
                                effects.FadeOut.Active = true;
                                effects.FadeOut.Duration = input->FadeOutDuration;
                            }
                        });
                        if (!input->MidiFile.IsEmpty())
                        {
                            const auto midiFile = std::make_shared<NH::Bass::MidiFile>(theme->GetName(), NH::String(input->MidiFile.ToChar()));
                            theme->AddMidiFile("", midiFile);
                        }
                        NH::Bass::Engine::GetInstance()->GetMusicManager().RefreshTheme(theme->GetName());
                    });
        }

        template<typename T>
        void ForEachClass(const zSTRING& className, const std::function<T*()>& classFactory, const std::function<void(T*, zCPar_Symbol*)>& instanceFunc)
        {
            ForEachPrototype(className, [&](const int index) {
                if (T* theme = classFactory())
                {
                    m_Parser->CreatePrototype(index, theme);
                }
            });
            ForEachInstance(className, [&](const int index, zCPar_Symbol* symbol) {
                T* theme = classFactory();
                if (theme)
                {
                    m_Parser->CreateInstance(index, theme);
                }
                instanceFunc(theme, symbol);
            });
        }

        void ForEachPrototype(const zSTRING& className, const std::function<void(int)>& func) const
        {
            const int classIndex = m_Parser->GetIndex(className);
            if (classIndex < 0)
            {
                return;
            }

            int prototypeIndex = m_Parser->GetPrototype(classIndex, 0);
            while (prototypeIndex > 0)
            {
                func(prototypeIndex);
                prototypeIndex = m_Parser->GetPrototype(classIndex, prototypeIndex + 1);
            }
        }

        void ForEachInstance(const zSTRING& className, const std::function<void(int, zCPar_Symbol*)>& func) const
        {
            const int classIndex = m_Parser->GetIndex(className);
            if (classIndex < 0)
            {
                return;
            }

            int symbolIndex = m_Parser->GetInstance(classIndex, 0);
            while (symbolIndex > 0)
            {
                zCPar_Symbol* symbol = m_Parser->GetSymbol(symbolIndex);
                func(symbolIndex, symbol);
                symbolIndex = m_Parser->GetInstance(classIndex, symbolIndex + 1);
            }
        }
    };
}// namespace GOTHIC_NAMESPACE