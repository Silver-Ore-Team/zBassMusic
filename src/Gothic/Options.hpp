#include "NH/Bass/Options.h"

namespace GOTHIC_NAMESPACE
{
    void ApplyOptions()
    {
        if (GetGameVersion() != ENGINE)
        {
            return;
        }

        Union::Array<NH::Logger*> loggers = NH::LoggerFactory::GetInstance()->GetLoggers();
        {
            static NH::Logger* log = NH::CreateLogger("zBassMusic::GothicOptions");
            zSTRING unionValue = zoptions->ReadString("BASSMUSIC", "LoggerLevelUnion", "INFO");
            zSTRING zSpyValue = zoptions->ReadString("BASSMUSIC", "LoggerLevelZSpy", "DEBUG");

            for (auto it = loggers.begin(); it != loggers.end(); it++)
            {
                auto* unionAdapter = (*it)->GetAdapter<NH::UnionConsoleLoggerAdapter>();
                if (unionAdapter)
                {
                    NH::LoggerLevel level = NH::StringToLoggerLevel(unionValue.ToChar());
                    if (level != unionAdapter->DEFAULT_LEVEL)
                    {
                        NH::UnionConsoleLoggerAdapter::DEFAULT_LEVEL = level;
                        log->Info("Logger level for UnionConsoleLoggerAdapter: {0}", unionValue.ToChar());
                    }
                    unionAdapter->SetLoggerLevel(level);
                }
                auto* zSpyAdapter = (*it)->GetAdapter<NH::ZSpyLoggerAdapter>();
                if (zSpyAdapter)
                {
                    NH::LoggerLevel level = NH::StringToLoggerLevel(zSpyValue.ToChar());
                    if (level != zSpyAdapter->DEFAULT_LEVEL)
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
    }
}
