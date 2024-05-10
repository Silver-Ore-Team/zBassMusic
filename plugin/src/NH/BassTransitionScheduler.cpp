#include "BassTransitionScheduler.h"

#include <utility>

namespace NH::Bass
{
    void TransitionScheduler::Schedule(Channel& activeChannel, const MusicDef& nextMusic)
    {
        const MusicDef& music = activeChannel.CurrentMusic();
        log->Trace("Schedule for {0} -> {1}", music.Filename, nextMusic.Filename);
        TransitionScheduleRule rule = GetScheduleRule(music);

        if (rule.Type == TransitionScheduleType::INSTANT)
        {
            log->Trace("Schedule rule type = INSTANT");
            m_Monitors.emplace_back(ScheduleMonitor{ &activeChannel, 0, [&nextMusic](const std::function<void(const MusicDef&)>& onReady) {
                onReady(nextMusic);
            }});
            return;
        }

        if (rule.Type == TransitionScheduleType::ON_BEAT)
        {
            log->Trace("Schedule rule type = ON_BEAT");
            TransitionScheduleRule::DataOnBeat data = std::get<TransitionScheduleRule::DataOnBeat>(rule.Data);

            log->Trace("OnBeat.Interval = {0}", data.Interval);
            log->Trace("OnBeat.TimePoints.Count = {0}", data.TimePoints.size());

            double overhead = music.EndTransition.Type != TransitionType::NONE ? music.EndTransition.Duration : 0;
            log->Trace("overhead = {0}", overhead);
            std::sort(data.TimePoints.begin(), data.TimePoints.end());

            double length = activeChannel.CurrentLength();
            double currentPosition = activeChannel.CurrentPosition();
            log->Trace("length = {0}", length);
            log->Trace("currentPosition = {0}", currentPosition);
            double timePointCandidate = -1;
            double intervalCandidate = - 1;

            log->Trace("Rule checking TimePoints");
            for (const auto &item: data.TimePoints)
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
                m_Monitors.emplace_back(ScheduleMonitor{ &activeChannel, 0, [&music](const std::function<void(const MusicDef&)>& onReady) {
                    onReady(music);
                }});
                return;
            }

            double target = timePointCandidate > intervalCandidate ? timePointCandidate : intervalCandidate;
            log->Trace("target = {0}", target);

            log->Debug("Starting Monitor {0} -> {1}, position = {2}", music.Filename, nextMusic.Filename, target);
            m_Monitors.emplace_back(ScheduleMonitor{&activeChannel, target, [&nextMusic](const std::function<void(const MusicDef&)>& onReady) {
                onReady(nextMusic);
            }});

            return;
        }

        log->Error("Unknown Schedule type = {0}", (size_t) rule.Type);
    }

    void TransitionScheduler::Update(const std::function<void(const MusicDef&)>& onReady)
    {
        for (auto &monitor : m_Monitors)
        {
            if (monitor.Position <= monitor.Channel->CurrentPosition())
            {
                if (monitor.Position > 0)
                {
                    double target = monitor.Position;
                    double position = monitor.Channel->CurrentPosition();
                    double delay = position - target;
                    log->Debug("Monitor for {0} completed, calling onReady\n  target = {1}\n  current = {2}\n  delay = {3}",
                               monitor.Channel->CurrentMusic().Filename, target, position, delay);
                }
                monitor.Done = true;
                monitor.Action(onReady);
            }
        }

        std::erase_if(m_Monitors, [](ScheduleMonitor& monitor) { return monitor.Done; });
    }

    void TransitionScheduler::AddRuleOnBeat(const char* name, double interval, std::vector<double> timePoints)
    {
        log->Debug("AddRule OnBeat for {0}, interval = {1}, timePoints.count = {2}", name, interval, timePoints.size());
        m_ScheduleRules.insert_or_assign(name, TransitionScheduleRule::OnBeat(interval, std::move(timePoints)));
    }

    const TransitionScheduleRule& TransitionScheduler::GetScheduleRule(const MusicDef& music)
    {
        const char* name = music.Filename.ToChar();
        if (m_ScheduleRules.contains(name))
        {
            return m_ScheduleRules.at(name);
        }

        static TransitionScheduleRule defaultRule = TransitionScheduleRule::Instant();
        return defaultRule;
    }
}