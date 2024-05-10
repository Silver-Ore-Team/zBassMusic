#include <NH/BassOptions.h>

namespace GOTHIC_NAMESPACE {
    void ApplyOptions() {
        if (GetGameVersion() != ENGINE) {
            return;
        }

        Union::Array<NH::Logger *> loggers = NH::LoggerFactory::GetInstance()->GetLoggers();
        {
            zSTRING unionValue = zoptions->ReadString("BASSMUSIC", "LoggerLevelUnion", "INFO");
            zSTRING zSpyValue = zoptions->ReadString("BASSMUSIC", "LoggerLevelZSpy", "DEBUG");

            for (auto it = loggers.begin(); it != loggers.end(); it++)
            {
                auto* unionAdapter = (*it)->GetAdapter<NH::UnionConsoleLoggerAdapter>();
                if (unionAdapter)
                {
                    unionAdapter->SetLoggerLevel(NH::StringToLoggerLevel(unionValue.ToChar()));
                }
                auto* zSpyAdapter = (*it)->GetAdapter<NH::ZSpyLoggerAdapter>();
                if (zSpyAdapter)
                {
                    zSpyAdapter->SetLoggerLevel(NH::StringToLoggerLevel(zSpyValue.ToChar()));
                }
            }
        }

        NH::Bass::Options->TransitionTime = zoptions->ReadReal("BASSMUSIC", "TransitionTime", 2000.0f);
        NH::Bass::Options->ForceDisableReverb = zoptions->ReadBool("BASSMUSIC", "ForceDisableReverb", false);
        NH::Bass::Options->ForceFadeTransition = zoptions->ReadBool("BASSMUSIC", "ForceFadeTransition", false);
        NH::Bass::Options->CreateMainParserCMusicTheme = zoptions->ReadBool("BASSMUSIC", "CreateMainParserCMusicTheme", true);
    }
}
