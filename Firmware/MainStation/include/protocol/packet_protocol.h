#ifndef PACKET_PROTOCOL_H
#define PACKET_PROTOCOL_H

#include <stdint.h>
#include "core/types/system_types.h"

constexpr uint8_t PROTOCOL_VERSION = 1;

enum class PacketType : uint8_t
{
    HEARTBEAT = 0x01,
    ACK = 0x02,
    ERROR = 0x03,
    DEVICE_STATUS = 0x04,
    VET_EVENT = 0x10,
    MILKING_START = 0x20,
    MILKING_UPDATE = 0x21,
    MILKING_FINISH = 0x22
};

#pragma pack(push, 1)

struct PacketHeader
{
    uint8_t protocolVersion;
    DeviceType source;
    PacketType type;
    uint16_t packetID;
    uint16_t sessionID;
    uint32_t timestamp;
    uint16_t payloadLength;
};

struct VetPayload
{
    uint32_t cowID;
    uint16_t eventCode;
    uint32_t operatorID;
};

struct MilkingPayload
{
    uint32_t cowID;
    float currentWeight;
    float temperature;
    uint32_t durationSeconds;
};

union PacketPayload
{
    VetPayload vet;
    MilkingPayload milking;
    uint8_t raw[64];
};

struct SmartPacket
{
    PacketHeader header;
    PacketPayload payload;
};

#pragma pack(pop)

// التأكد من عدم تجاوز الحزمة للحد الأقصى لـ ESP-NOW (250 bytes)
static_assert(sizeof(SmartPacket) <= 250, "SmartPacket exceeds ESP-NOW maximum payload size!");

#endif // PACKET_PROTOCOL_H
