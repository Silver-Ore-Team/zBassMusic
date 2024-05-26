#pragma once

#include "NH/Bass/CommonTypes.h"
#include "NH/Logger.h"
#include "MusicTheme.h"
#include <NH/HashString.h>
#include <utility>
#include <vector>
#include <deque>
#include <variant>

namespace NH::Bass
{

    enum class EventType
    {
        UNKNOWN = 0,
        MUSIC_END,
        MUSIC_TRANSITION,
        MUSIC_CHANGE
    };

    struct Event
    {
        struct MusicEnd { std::shared_ptr<MusicTheme> Theme; HashString AudioId; };
        struct MusicTransition { std::shared_ptr<MusicTheme> Theme; HashString AudioId; float TimeLeft; };
        struct MusicChange { std::shared_ptr<MusicTheme> Theme; HashString AudioId; };
        using DataType = std::variant<MusicEnd, MusicTransition, MusicChange>;

        EventType Type = EventType::UNKNOWN;
        DataType Data;

        Event() = delete;
        Event(EventType type, DataType data) : Type(type), Data(std::move(data)) {}
    };

    using EventSubscriberFn = void (*)(const Event&, void*);

    struct EventSubscriber
    {
        EventType Type = EventType::UNKNOWN;
        EventSubscriberFn Function{};
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

        friend Engine;

        std::vector<EventSubscriber> m_Subscribers{};
        std::deque<Event> m_EventQueue;

    public:
        EventSubscriber* AddSubscriber(EventType type, EventSubscriberFn function, void* userData = nullptr);

        void RemoveSubscriber(const EventSubscriber* subscriber);

        void DispatchEvent(Event&& event);

        void DispatchEvent(const Event& event);

        void Update();

    private:
        EventManager() = default;
    };

    struct MusicEndEvent : public Event
    {
        MusicEnd Data;
        MusicEndEvent(std::shared_ptr<MusicTheme> theme, HashString audioId) : Event(EventType::MUSIC_END, MusicEnd{std::move(theme), audioId}) {}
    };

    struct MusicTransitionEvent : public Event
    {
        MusicTransition Data;
        MusicTransitionEvent(std::shared_ptr<MusicTheme> theme, HashString audioId, float timeLeft)
            : Event(EventType::MUSIC_TRANSITION, MusicTransition{std::move(theme), audioId, timeLeft}) {}
    };

    struct MusicChangeEvent : public Event
    {
        MusicChange Data;
        MusicChangeEvent(std::shared_ptr<MusicTheme> theme, HashString audioId) : Event(EventType::MUSIC_CHANGE, MusicChange{std::move(theme), audioId}) {}
    };
}