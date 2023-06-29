#pragma once

#include <NH/BassTypes.h>
#include <Union/Array.h>

namespace NH
{
	namespace Bass
	{
		using EventSubscriberFunction = void(*)(const MusicDef*, int data, void*);

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
			friend Engine;

			Union::Array<EventSubscriber> m_Subscribers{};

		public:
			EventSubscriber* AddSubscriber(EventType type, EventSubscriberFunction function, void* userData = nullptr);

			void RemoveSubscriber(const EventSubscriber* subscriber);

		private:
			EventManager();

			void DispatchEvent(EventType type, const MusicDef* musicDef, int data = 0);
		};
	}
}