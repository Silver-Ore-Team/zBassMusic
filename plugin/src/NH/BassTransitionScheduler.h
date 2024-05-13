#pragma once

#include <NH/BassTypes.h>
#include <NH/BassChannel.h>
#include <NH/Logger.h>

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

	struct ScheduleMonitor
	{
		Channel* Channel;
		double Position;
		std::function<void(std::function < void( const MusicDef&)> onReady)> Action;
		bool Done = false;
	};

	class TransitionScheduler
	{
		NH::Logger* log = NH::CreateLogger("zBassMusic::TransitionScheduler");
		std::unordered_map<std::string, TransitionScheduleRule> m_ScheduleRules;
		std::vector<ScheduleMonitor> m_Monitors;

	public:
		void Schedule(Channel& activeChannel, const MusicDef& nextMusic);

		void Update(const std::function<void(const MusicDef&)>& onReady);

		void AddRuleOnBeat(const char* name, double interval = 0, std::vector<double> timePoints = {});

		const TransitionScheduleRule& GetScheduleRule(const MusicDef& music);
	};
}