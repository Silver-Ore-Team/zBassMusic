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

    struct BassMusicTheme
    {
        zSTRING Name;
        zSTRING Zones;
    };

    struct BassMusicThemeAudio
    {
        zSTRING Type;
        zSTRING Filename;
        zSTRING MidiFile;
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
        }

    private:
        void LoadGothic()
        {
            ForEachClass<GothicMusicTheme>(
                    "C_MUSICTHEME",
                    [&]() { return m_GothicThemeInstances.emplace_back(new GothicMusicTheme{}); },
                    [&](GothicMusicTheme* input, zCPar_Symbol* symbol)
                    {
                        std::shared_ptr<NH::Bass::MusicTheme> theme = std::make_shared<NH::Bass::MusicTheme>(symbol->name.ToChar());
                        theme->SetAudioFile(NH::Bass::AudioFile::DEFAULT, input->fileName.ToChar());
                        theme->SetAudioEffects(NH::Bass::AudioFile::DEFAULT, [&](NH::Bass::AudioEffects& effects)
                        {
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
                        theme->AddZone(symbol->name.ToChar());
                        NH::Bass::Engine::GetInstance()->GetMusicManager().AddTheme(symbol->name.ToChar(), theme);
                    });
        }

        void LoadBass()
        {
            ForEachClass<BassMusicTheme>(
                    "C_BassMusic_Theme",
                    [&]() { return m_BassThemeInstances.emplace_back(new BassMusicTheme{}); },
                    [&](BassMusicTheme* theme, zCPar_Symbol* symbol)
                    {
                        // @todo:
                    });

            ForEachClass<BassMusicThemeAudio>(
                    "C_BassMusic_ThemeAudio",
                    [&]() { return m_BassThemeAudioInstances.emplace_back(new BassMusicThemeAudio{}); },
                    [&](BassMusicThemeAudio* theme, zCPar_Symbol* symbol)
                    {
                        // @todo:
                    });
        }

        template<typename T>
        void ForEachClass(const zSTRING& className, const std::function<T*()>& classFactory, const std::function<void(T*, zCPar_Symbol*)>& instanceFunc)
        {
            ForEachPrototype(className, [&](int index)
            {
                T* theme = classFactory();
                if (theme)
                {
                    m_Parser->CreatePrototype(index, theme);
                }
            });
            ForEachInstance(className, [&](int index, zCPar_Symbol* symbol)
            {
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