#include "BassEventManager.h"

namespace NH
{
	namespace Bass
	{
		EventSubscriber* EventManager::AddSubscriber(const EventType type, const EventSubscriberFunction function, void* userData)
		{
			const int index = m_Subscribers.Insert({type, function, userData});
			return &m_Subscribers[index];
		}

		void EventManager::RemoveSubscriber(const EventSubscriber* subscriber)
		{
			for (auto s : m_Subscribers)
			{
				if (&s == subscriber)
				{
					m_Subscribers.Remove(s);
				}
			}
		}

		EventManager::EventManager()
		{
		}

		void EventManager::DispatchEvent(const EventType type, const MusicDef* musicDef, const int data)
		{
			for (const auto s : m_Subscribers)
			{
				if (s.Type == type)
				{
					s.Function(musicDef, data, s.UserData);
				}
			}
		}
	}
}