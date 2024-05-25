#include "EventManager.h"

namespace NH::Bass
{
    NH::Logger* EventManager::log = NH::CreateLogger("zBassMusic::EventManager");

    EventSubscriber*
    EventManager::AddSubscriber(const EventType type, const EventSubscriberFn function, void* userData)
    {
        return &m_Subscribers.emplace_back(EventSubscriber{ type, function, userData });
    }

    void EventManager::RemoveSubscriber(const EventSubscriber* subscriber)
    {
        erase_if(m_Subscribers, [&subscriber](const EventSubscriber& s)
        {
            return s == *subscriber;
        });
    }

    void EventManager::DispatchEvent(Event&& event)
    {
        m_EventQueue.emplace_front(event);
    }

    void EventManager::DispatchEvent(const Event& event)
    {
        m_EventQueue.emplace_front(event);
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
                if (s.Type == event.Type)
                {
                    s.Function(event, s.UserData);
                }
            }
        }
    }
}
