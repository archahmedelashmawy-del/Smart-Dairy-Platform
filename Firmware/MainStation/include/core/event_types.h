enum class EventType : uint16_t
{
    None = 0,

    // Communication
    PacketReceived,
    PacketSent,
    PacketDropped,
    InvalidPacket,

    // RFID
    RFIDDetected,

    // Milking
    MilkingStarted,
    MilkingStopped,

    // Veterinary
    VetEventReceived,

    // Storage
    SDMounted,
    SDWriteFailed,

    // System
    Error,
    Warning
};
