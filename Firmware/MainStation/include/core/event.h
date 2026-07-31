#ifndef EVENT_H
#define EVENT_H

#include <Arduino.h>
#include <stdint.1h>

#include "core/event_types.h"

/*
==========================================================
 Smart Dairy Platform
 System Event Contract
----------------------------------------------------------
 Official event object exchanged through EventBus.
==========================================================
*/

/*----------------------------------------------------------
    Event Source
----------------------------------------------------------*/

enum class EventSource : uint8_t
{
    Unknown = 0,

    System,
    StateMachine,

    Communication,

    RFID,

    Keypad,

    RTC,

    SDCard,

    TemperatureSensor,

    SuctionSensor,

    Relay,

    Servo,

    Buzzer,

    DesktopGateway,

    Cloud,

    Diagnostics
};

/*----------------------------------------------------------
    Event Priority
----------------------------------------------------------*/

enum class EventPriority : uint8_t
{
    Low = 0,

    Normal,

    High,

    Critical
};

/*----------------------------------------------------------
    Generic Event
----------------------------------------------------------*/

struct SystemEvent
{
    EventType type = EventType::None;

    EventSource source = EventSource::Unknown;

    EventPriority priority = EventPriority::Normal;

    uint32_t timestamp = 0;

    const void* payload = nullptr;

    size_t payloadSize = 0;
};

#endif
