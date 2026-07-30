#include "core/event_bus.h"

/*----------------------------------------------------------
    Static Members
----------------------------------------------------------*/

EventCallback EventBus::subscribers[SystemConstants::MAX_EVENT_SUBSCRIBERS];

size_t EventBus::count = 0;

bool EventBus::isPublishing = false;

/*----------------------------------------------------------
    Initialization
----------------------------------------------------------*/

void EventBus::initialize()
{
    clear();
}

/*----------------------------------------------------------
    Subscribe
----------------------------------------------------------*/

bool EventBus::subscribe(EventCallback callback)
{
    if (callback == nullptr)
        return false;

    if (isPublishing)
        return false;

    if (count >= SystemConstants::MAX_EVENT_SUBSCRIBERS)
        return false;

    if (isSubscribed(callback))
        return true;

    subscribers[count++] = callback;

    return true;
}

/*----------------------------------------------------------
    Unsubscribe
----------------------------------------------------------*/

bool EventBus::unsubscribe(EventCallback callback)
{
    if (isPublishing)
        return false;

    for (size_t i = 0; i < count; ++i)
    {
        if (subscribers[i] == callback)
        {
            for (size_t j = i; j + 1 < count; ++j)
            {
                subscribers[j] = subscribers[j + 1];
            }

            subscribers[count - 1] = nullptr;

            --count;

            return true;
        }
    }

    return false;
}

/*----------------------------------------------------------
    Publish
----------------------------------------------------------*/

void EventBus::publish(const SystemEvent& event)
{
    isPublishing = true;

    for (size_t i = 0; i < count; ++i)
    {
        if (subscribers[i] != nullptr)
        {
            subscribers[i](event);
        }
    }

    isPublishing = false;
}

/*----------------------------------------------------------
    Clear
----------------------------------------------------------*/

void EventBus::clear()
{
    for (size_t i = 0; i < SystemConstants::MAX_EVENT_SUBSCRIBERS; ++i)
    {
        subscribers[i] = nullptr;
    }

    count = 0;
    isPublishing = false;
}

/*----------------------------------------------------------
    Information
----------------------------------------------------------*/

size_t EventBus::subscriberCount()
{
    return count;
}

bool EventBus::isSubscribed(EventCallback callback)
{
    for (size_t i = 0; i < count; ++i)
    {
        if (subscribers[i] == callback)
            return true;
    }

    return false;
}
