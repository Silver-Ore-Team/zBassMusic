#pragma once

#include "NH/Bass/CommonTypes.h"
#include "Channel.h"
#include "NH/Logger.h"

#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#include <functional>
#include <string>

namespace NH::Bass
{
    enum class TransitionScheduleType
    {
        // Performs the transition instantly when it's ready
        INSTANT,
        // Performs the transition only on defined timecodes (like beat grops)
        ON_BEAT
    };

    struct TransitionScheduleRule
    {
        struct DataInstant
        {
        };

        struct DataOnBeat
        {
            // Static interval when transition will be performed, in seconds.
            // BeatInterval = 1.2; means that transition may happen only at {1.2, 2.4, 3.6, 4.8, ...} time points.
            // Non-positive value disables static interval.
            double Interval = 0;

            // Vector of time points in seconds when the transiton will be performed.
            std::vector<double> TimePoints;
        };

        struct DataJingle
        {

        };

        TransitionScheduleType Type;
        std::variant<DataInstant, DataOnBeat> Data;

        static TransitionScheduleRule Instant()
        {
            return { TransitionScheduleType::INSTANT };
        }

        static TransitionScheduleRule OnBeat(double interval = 0, std::vector<double> timePoints = {})
        {
            return { TransitionScheduleType::ON_BEAT, DataOnBeat{ interval, std::move(timePoints) }};
        }
    };

    struct ScheduledAction
    {
        std::shared_ptr<Channel> Channel;
        double Position;
        std::function<void(Engine&)> Action;
        bool Done = false;
    };

    class TransitionScheduler
    {
        NH::Logger* log = NH::CreateLogger("zBassMusic::TransitionScheduler");
        std::unordered_map<HashString, TransitionScheduleRule> m_ScheduleRules;
        std::vector<ScheduledAction> m_ScheduledActions;

    public:
        void Schedule(const std::shared_ptr<Channel>& activeChannel, const std::shared_ptr<MusicTheme>& nextMusic);

        void Update(Engine& engine);

        void AddRuleOnBeat(const char* name, double interval = 0, std::vector<double> timePoints = {});

    private:
        const TransitionScheduleRule& GetScheduleRule(HashString id);

        void ScheduleInstant(const std::shared_ptr<Channel>& activeChannel, const std::shared_ptr<MusicTheme>& nextMusic);

        void ScheduleOnBeat(const std::shared_ptr<Channel>& activeChannel, const std::shared_ptr<MusicTheme>& nextMusic, const TransitionScheduleRule& rule);
    };
}