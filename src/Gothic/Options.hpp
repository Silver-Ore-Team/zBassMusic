#include "NH/Bass/Options.h"

namespace GOTHIC_NAMESPACE
{
    inline void ApplyOptions()
    {
        if (GetGameVersion() != ENGINE)
        {
            return;
        }

        {
            Union::Array<NH::Logger*> loggers = NH::LoggerFactory::GetInstance()->GetLoggers();
            static NH::Logger* log = NH::CreateLogger("zBassMusic::GothicOptions");
            const zSTRING unionValue = zoptions->ReadString("BASSMUSIC", "LoggerLevelUnion", "INFO");
            const zSTRING zSpyValue = zoptions->ReadString("BASSMUSIC", "LoggerLevelZSpy", "DEBUG");

            for (const auto& logger : loggers)
            {
                if (auto* unionAdapter = logger->GetAdapter<NH::UnionConsoleLoggerAdapter>())
                {
                    const NH::LoggerLevel level = NH::StringToLoggerLevel(unionValue.ToChar());
                    if (level != NH::UnionConsoleLoggerAdapter::DEFAULT_LEVEL)
                    {
                        NH::UnionConsoleLoggerAdapter::DEFAULT_LEVEL = level;
                        log->Info("Logger level for UnionConsoleLoggerAdapter: {0}", unionValue.ToChar());
                    }
                    unionAdapter->SetLoggerLevel(level);
                }
                if (auto* zSpyAdapter = logger->GetAdapter<NH::ZSpyLoggerAdapter>())
                {
                    const NH::LoggerLevel level = NH::StringToLoggerLevel(zSpyValue.ToChar());
                    if (level != NH::ZSpyLoggerAdapter::DEFAULT_LEVEL)
                    {
                        NH::ZSpyLoggerAdapter::DEFAULT_LEVEL = level;
                        log->Info("Logger level for ZSpyLoggerAdapter: {0}", unionValue.ToChar());
                    }
                    zSpyAdapter->SetLoggerLevel(level);
                }
            }
        }

        NH::Bass::Options->TransitionTime = zoptions->ReadReal("BASSMUSIC", "TransitionTime", 2000.0f);
        NH::Bass::Options->ForceDisableReverb = zoptions->ReadBool("BASSMUSIC", "ForceDisableReverb", false);
        NH::Bass::Options->ForceFadeTransition = zoptions->ReadBool("BASSMUSIC", "ForceFadeTransition", false);
        NH::Bass::Options->CreateMainParserCMusicTheme = zoptions->ReadBool("BASSMUSIC", "CreateMainParserCMusicTheme", true);
        NH::Bass::Options->LazyLoading = zoptions->ReadBool("BASSMUSIC", "LazyLoading", true);
        NH::Bass::Options->MaxLoadedThemes = zoptions->ReadInt("BASSMUSIC", "MaxLoadedThemes", 5);
        if (NH::Bass::Options->MaxLoadedThemes < 1)
        {
            NH::Bass::Options->MaxLoadedThemes = 1;
        }
    }

    void UpdateMusicOptions()
    {
        if (zmusic)
        {
            float volume = zoptions->ReadReal("SOUND", "musicVolume", 1.0f);
            if (zmusic->GetVolume() != volume)
            {
                zmusic->SetVolume(volume);
            }
            int musicDisabled = !zoptions->ReadInt("SOUND", "musicEnabled", 1);
            if (musicDisabled != zCMusicSystem::s_musicSystemDisabled)
            {
                zCMusicSystem::DisableMusicSystem(musicDisabled);
                zCZoneMusic::SetAutochange(!musicDisabled);
            }

        }
    }


}
