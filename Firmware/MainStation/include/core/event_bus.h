#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stddef.h>

#include "config/constants.h"
#include "core/event.h"

/*
==========================================================
 Smart Dairy Platform
 Event Bus
----------------------------------------------------------
 Central publish / subscribe mechanism.
==========================================================
*/

using EventCallback = void (*)(const SystemEvent&);

class EventBus
{
public:

    static void initialize();

    [[nodiscard]] static bool subscribe(EventCallback callback);

    [[nodiscard]] static bool unsubscribe(EventCallback callback);

    static void publish(const SystemEvent& event);

    static void clear();

    [[nodiscard]] static size_t subscriberCount();

    [[nodiscard]] static bool isSubscribed(EventCallback callback);

private:

    static EventCallback subscribers[SystemConstants::MAX_EVENT_SUBSCRIBERS];

    static size_t count;

    static bool isPublishing;
};

#endif
