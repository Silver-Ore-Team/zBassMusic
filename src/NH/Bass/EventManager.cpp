#include "EventManager.h"

namespace NH::Bass
{
	NH::Logger* EventManager::log = NH::CreateLogger("zBassMusic::EventManager");

	EventSubscriber*
	EventManager::AddSubscriber(const EventType type, const EventSubscriberFunction function, void* userData)
	{
		const int index = m_Subscribers.Insert({ type, function, userData });
		return &m_Subscribers[index];
	}

	void EventManager::RemoveSubscriber(const EventSubscriber* subscriber)
	{
		for (const auto i: std::views::iota(0u, m_Subscribers.GetCount()) | std::views::reverse)
		{
			if (&m_Subscribers[i] == subscriber)
			{
				m_Subscribers.RemoveAt(i);
			}
		}
	}

	void EventManager::DispatchEvent(const EventType type, const MusicDef& musicDef, const int data)
	{
		m_EventQueue.emplace_front(Event{ type, musicDef, data });
	}

	void EventManager::Update()
	{
		while (!m_EventQueue.empty())
		{
			log->Debug("Processing {0} events", m_EventQueue.size());
			Event event = m_EventQueue.back();
			m_EventQueue.pop_back();
			for (const auto& s: m_Subscribers)
			{
				if (s.Type == event.type)
				{
					s.Function(event.music, event.data, s.UserData);
				}
			}
		}
	}
}
