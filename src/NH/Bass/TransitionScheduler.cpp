#include "TransitionScheduler.h"

#include <NH/Bass/Engine.h>
#include <memory>
#include <utility>

namespace NH::Bass
{
    void TransitionScheduler::Schedule(const std::shared_ptr<Channel>& activeChannel, const std::shared_ptr<MusicTheme>& nextMusic)
    {
        if (!activeChannel)
        {
            m_ScheduledActions.emplace_back(ScheduledAction{ activeChannel, 0, [id = HashString(nextMusic->GetName())](Engine& engine)
            {
                engine.GetCommandQueue().AddCommand(std::make_shared<PlayThemeCommand>(id, AudioFile::DEFAULT));
            }});
            return;
        }

        const auto& currentTheme = activeChannel->CurrentTheme();
        const TransitionScheduleRule& rule = GetScheduleRule(currentTheme->GetName());

        if (rule.Type == TransitionScheduleType::INSTANT)
        {
            ScheduleInstant(activeChannel, nextMusic);
            return;
        }

        if (rule.Type == TransitionScheduleType::ON_BEAT)
        {
            ScheduleOnBeat(activeChannel, nextMusic, rule);
            return;
        }

        log->Error("Unknown Schedule type = {0}", (size_t)rule.Type);
    }

    void TransitionScheduler::ScheduleInstant(const std::shared_ptr<Channel>& activeChannel, const std::shared_ptr<MusicTheme>& nextMusic)
    {
        m_ScheduledActions.emplace_back(ScheduledAction{ activeChannel, 0, [id = HashString(nextMusic->GetName())](Engine& engine)
        {
            engine.GetCommandQueue().AddCommand(std::make_shared<PlayThemeCommand>(id, AudioFile::DEFAULT));
        }});
    }

    void TransitionScheduler::ScheduleOnBeat(const std::shared_ptr<Channel>& activeChannel, const std::shared_ptr<MusicTheme>& nextMusic, const TransitionScheduleRule& rule)
    {
        TransitionScheduleRule::DataOnBeat data = std::get<TransitionScheduleRule::DataOnBeat>(rule.Data);
        const auto& currentTheme = activeChannel->CurrentTheme();
        const auto& effects = currentTheme->GetAudioEffects(AudioFile::DEFAULT);

        double overhead = effects.FadeOut.Active ? effects.FadeOut.Duration : 0;
        std::sort(data.TimePoints.begin(), data.TimePoints.end());

        double length = activeChannel->CurrentLength();
        double currentPosition = activeChannel->CurrentPosition();
        log->Trace("length = {0}", length);
        log->Trace("currentPosition = {0}", currentPosition);
        double timePointCandidate = -1;
        double intervalCandidate = -1;

        log->Trace("Rule checking TimePoints");
        for (const auto& item: data.TimePoints)
        {
            double minValue = currentPosition - overhead;
            if (item > minValue)
            {
                log->Trace("found TimePoint = {0}", item);
                timePointCandidate = item;
                break;
            }
        }

        if (data.Interval > 0)
        {
            log->Trace("Rule checking Interval");

            // Please don't abort me. Quick hack for preview. Fast enough.
            for (double time = 0; time < length; time += data.Interval)
            {
                double minValue = currentPosition - overhead;
                if (time > minValue)
                {
                    log->Trace("found TimePoint = {0}", time);
                    intervalCandidate = time;
                    break;
                }
            }
        }

        if (timePointCandidate <= 0 && intervalCandidate <= 0)
        {
            log->Trace("Not found any candidates, rollback to INSTANT");
            ScheduleInstant(activeChannel, nextMusic);
            return;
        }

        double target = timePointCandidate > intervalCandidate ? timePointCandidate : intervalCandidate;
        log->Trace("target = {0}", target);

        log->Debug("Starting Monitor {0} -> {1}, position = {2}", currentTheme->GetName(), nextMusic->GetName(), target);
        m_ScheduledActions.emplace_back(ScheduledAction{ activeChannel, target, [id = HashString(nextMusic->GetName())](Engine& engine)
        {
            engine.GetCommandQueue().AddCommand(std::make_shared<PlayThemeCommand>(id, AudioFile::DEFAULT));
        }});
    }

    void TransitionScheduler::Update(Engine& engine)
    {
        for (auto& scheduledAction : m_ScheduledActions)
        {
            if (!scheduledAction.Channel)
            {
                scheduledAction.Done = true;
                scheduledAction.Action(engine);
                continue;
            }

            if (scheduledAction.Position <= scheduledAction.Channel->CurrentPosition())
            {
                if (scheduledAction.Position > 0)
                {
                    double target = scheduledAction.Position;
                    double position = scheduledAction.Channel->CurrentPosition();
                    double delay = position - target;
                    log->Debug(
                            "Monitor for {0} completed, calling onReady\n  target = {1}\n  current = {2}\n  delay = {3}",
                            scheduledAction.Channel->CurrentTheme()->GetName(), target, position, delay);
                }
                scheduledAction.Done = true;
                scheduledAction.Action(engine);
            }
        }

        std::erase_if(m_ScheduledActions, [](const ScheduledAction& monitor) { return monitor.Done; });
    }

    void TransitionScheduler::AddRuleOnBeat(const char* name, double interval, std::vector<double> timePoints)
    {
        log->Debug("AddRule OnBeat for {0}, interval = {1}, timePoints.count = {2}", name, interval, timePoints.size());
        m_ScheduleRules.insert_or_assign(name, TransitionScheduleRule::OnBeat(interval, std::move(timePoints)));
    }

    const TransitionScheduleRule& TransitionScheduler::GetScheduleRule(HashString id)
    {
        if (m_ScheduleRules.contains(id))
        {
            return m_ScheduleRules.at(id);
        }

        static TransitionScheduleRule defaultRule = TransitionScheduleRule::Instant();
        return defaultRule;
    }
}