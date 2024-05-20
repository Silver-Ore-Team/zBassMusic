#pragma once

#include "NH/Bass/CommonTypes.h"
#include "NH/Logger.h"
#include <vector>
#include <deque>

namespace NH::Bass
{
	using EventSubscriberFunction = void (*)(const MusicDef&, int data, void*);

	enum class EventType
	{
		UNKNOWN = 0,
		MUSIC_END,
		MUSIC_TRANSITION,
		MUSIC_CHANGE
	};

	struct EventSubscriber
	{
		EventType Type = EventType::UNKNOWN;
		EventSubscriberFunction Function{};
		void* UserData = nullptr;

		bool operator==(const EventSubscriber& other) const
		{
			return this == &other;
		}
	};

	class Engine;

	class EventManager
	{
		static NH::Logger* log;

		struct Event
		{
			EventType type = EventType::UNKNOWN;
			MusicDef music;
			int data{};
		};

		friend Engine;

		std::vector<EventSubscriber> m_Subscribers{};
		std::deque<Event> m_EventQueue;

	public:
		EventSubscriber* AddSubscriber(EventType type, EventSubscriberFunction function, void* userData = nullptr);

		void RemoveSubscriber(const EventSubscriber* subscriber);

		void DispatchEvent(EventType type, const MusicDef& musicDef, int data = 0);

		void Update();

	private:
		EventManager() = default;
	};
}