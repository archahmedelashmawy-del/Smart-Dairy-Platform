#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include <stdint.h>

/*
==========================================================
 Smart Dairy Platform
 Event Type Definitions
----------------------------------------------------------
 Central list of every event that may circulate
 through the EventBus.
==========================================================
*/

enum class EventType : uint16_t
{
    None = 0,

    /*--------------------------------------------------
        System Events
    --------------------------------------------------*/
    SystemStartup,
    SystemShutdown,
    SystemReset,
    SystemHeartbeat,

    /*--------------------------------------------------
        Communication
    --------------------------------------------------*/
    PacketReceived,
    PacketSent,
    PacketDropped,
    PacketValidationFailed,
    PeerConnected,
    PeerDisconnected,

    /*--------------------------------------------------
        RFID
    --------------------------------------------------*/
    RFIDCardDetected,
    RFIDReadFailed,

    /*--------------------------------------------------
        User Input
    --------------------------------------------------*/
    KeyPressed,

    /*--------------------------------------------------
        Milking Process
    --------------------------------------------------*/
    MilkingRequested,
    MilkingStarted,
    MilkingRunning,
    MilkingPaused,
    MilkingFinished,
    MilkingCancelled,

    /*--------------------------------------------------
        Pump
    --------------------------------------------------*/
    PumpStartRequested,
    PumpStarted,
    PumpStopped,

    /*--------------------------------------------------
        Servo
    --------------------------------------------------*/
    ServoOpened,
    ServoClosed,

    /*--------------------------------------------------
        Sensors
    --------------------------------------------------*/
    TemperatureUpdated,
    SuctionUpdated,
    FlowUpdated,

    /*--------------------------------------------------
        Storage
    --------------------------------------------------*/
    SDMounted,
    SDRemoved,
    DataSaved,
    ExportCompleted,

    /*--------------------------------------------------
        RTC
    --------------------------------------------------*/
    TimeUpdated,

    /*--------------------------------------------------
        Diagnostics
    --------------------------------------------------*/
    WarningRaised,
    ErrorRaised,

    /*--------------------------------------------------
        Future Expansion
    --------------------------------------------------*/
    DatabaseSync,
    CloudSync,
    OTAStarted,
    OTAFinished
};

#endif
