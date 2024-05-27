#include <functional>
#include <NH/Bass/MidiFile.h>

namespace GOTHIC_NAMESPACE
{
    struct GothicMusicTheme
    {
        zSTRING fileName;
        float vol;
        bool loop;
        float reverbMix;
        float reverbTime;
        zTMus_TransType trType;
        zTMus_TransSubType trSubType;
    };

    enum class BassMusicThemeType : size_t { NORMAL = 0 };

    struct BassMusicTheme
    {
        zSTRING Name;
        zSTRING Zones;
        BassMusicThemeType Type = BassMusicThemeType::NORMAL;
    };

    struct BassMusicThemeAudio
    {
        zSTRING Theme;
        zSTRING Type;
        zSTRING Filename;
        zSTRING MidiFile;
        float Volume;
        int Loop;
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
            for (auto theme: m_GothicThemeInstances) { delete theme; }
            for (auto theme: m_BassThemeInstances) { delete theme; }
            for (auto theme: m_BassThemeAudioInstances) { delete theme; }
        }

        void Load()
        {
            LoadGothic();
            LoadBass();

            zSTRING initFuncName("BassMusic_Init");
            zCPar_Symbol* symbol = m_Parser->GetSymbol(initFuncName);
            if (symbol && symbol->type == zPAR_TYPE_FUNC)
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
                    [&](GothicMusicTheme* input, zCPar_Symbol* symbol) {
                        std::shared_ptr<NH::Bass::MusicTheme> theme = std::make_shared<NH::Bass::MusicTheme>(symbol->name.ToChar());
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
                            bool forceFade = NH::Bass::Options->ForceFadeTransition;
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
            ForEachClass<BassMusicTheme>(
                    Globals->BassMusicThemeClassName,
                    [&]() { return m_BassThemeInstances.emplace_back(new BassMusicTheme{}); },
                    [&](BassMusicTheme* input, zCPar_Symbol* symbol) {
                        std::shared_ptr<NH::Bass::MusicTheme> theme = std::make_shared<NH::Bass::MusicTheme>(input->Name.ToChar());
                        theme->SetAudioEffects(NH::Bass::AudioFile::DEFAULT, [](NH::Bass::AudioEffects& effects){});
                        auto zones = NH::String(input->Zones.ToChar()).Split(",");
                        for (auto& zone: zones) { theme->AddZone(zone.MakeUpper()); }
                        // input->Type ignored for now
                        NH::Bass::Engine::GetInstance()->GetMusicManager().AddTheme(input->Name.ToChar(), theme);
                    });

            ForEachClass<BassMusicThemeAudio>(
                    Globals->BassMusicThemeAudioClassName,
                    [&]() { return m_BassThemeAudioInstances.emplace_back(new BassMusicThemeAudio{}); },
                    [&](BassMusicThemeAudio* input, zCPar_Symbol* symbol) {
                        std::shared_ptr<NH::Bass::MusicTheme> theme = NH::Bass::Engine::GetInstance()->GetMusicManager().GetTheme(input->Theme.ToChar());
                        NH::String type =  NH::String(input->Type.ToChar());
                        NH::HashString id = type == "DEFAULT" ? NH::Bass::AudioFile::DEFAULT : NH::HashString(type);
                        theme->SetAudioFile(id, input->Filename.ToChar());
                        theme->SetAudioEffects(id, [&](NH::Bass::AudioEffects& effects) {
                            effects.Loop.Active = input->Loop;
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
                            auto midiFile = std::make_shared<NH::Bass::MidiFile>(theme->GetName(), NH::HashString(NH::String(input->MidiFile.ToChar())));
                            theme->AddMidiFile(NH::HashString(""), midiFile);
                        }
                        NH::Bass::Engine::GetInstance()->GetMusicManager().RefreshTheme(theme->GetName());
                    });
        }

        template<typename T>
        void ForEachClass(const zSTRING& className, const std::function<T*()>& classFactory, const std::function<void(T*, zCPar_Symbol*)>& instanceFunc)
        {
            ForEachPrototype(className, [&](int index) {
                T* theme = classFactory();
                if (theme)
                {
                    m_Parser->CreatePrototype(index, theme);
                }
            });
            ForEachInstance(className, [&](int index, zCPar_Symbol* symbol) {
                T* theme = classFactory();
                if (theme)
                {
                    m_Parser->CreateInstance(index, theme);
                }
                instanceFunc(theme, symbol);
            });
        }

        void ForEachPrototype(const zSTRING& className, const std::function<void(int)>& func)
        {
            int classIndex = m_Parser->GetIndex(className);
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

        void ForEachInstance(const zSTRING& className, const std::function<void(int, zCPar_Symbol*)>& func)
        {
            int classIndex = m_Parser->GetIndex(className);
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
}