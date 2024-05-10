#pragma once

#include <NH/BassTypes.h>
#include <NH/Logger.h>
#include <Union/Array.h>
#include <deque>

namespace NH
{
	namespace Bass
	{
		using EventSubscriberFunction = void(*)(const MusicDef&, int data, void*);

		enum class EventType
		{
			MUSIC_END,
			MUSIC_TRANSITION,
			MUSIC_CHANGE,
			MUSIC_ACTIVE
		};

		struct EventSubscriber
		{
			EventType Type;
			EventSubscriberFunction Function;
			void* UserData;

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
				EventType type;
				MusicDef music;
				int data;
			};

			friend Engine;

			Union::Array<EventSubscriber> m_Subscribers{};
			std::deque<Event> m_EventQueue;

		public:
			EventSubscriber* AddSubscriber(EventType type, EventSubscriberFunction function, void* userData = nullptr);

			void RemoveSubscriber(const EventSubscriber* subscriber);

			void DispatchEvent(EventType type, const MusicDef& musicDef, int data = 0);

			void Update();

		private:
			EventManager() {};
		};
	}
}