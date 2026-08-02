#ifndef PACKET_PROTOCOL_H
#define PACKET_PROTOCOL_H

#include <stdint.h>
#include "core/types/system_types.h"

// إصدار البروتوكول الحالي
constexpr uint8_t PROTOCOL_VERSION = 1;

// أنواع الحزم الشبكية للمشروع
enum class PacketType : uint8_t
{
    HEARTBEAT       = 0x01,
    ACK             = 0x02,
    ERROR           = 0x03,
    DEVICE_STATUS   = 0x04,
    VET_EVENT       = 0x10,
    MILKING_START   = 0x20,
    MILKING_UPDATE  = 0x21,
    MILKING_FINISH  = 0x22
};

#pragma pack(push, 1)

// الهيدر الموحد لجميع الحزم
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

// محتوى أحداث البيطري
struct VetPayload
{
    uint32_t cowID;
    uint16_t eventCode;
    uint32_t operatorID;
};

// محتوى بيانات الحلب
struct MilkingPayload
{
    uint32_t cowID;
    float currentWeight;
    float temperature;
    uint32_t durationSeconds;
};

// الاتحاد الخاص بمحتوى الحزمة (Payload Union)
union PacketPayload
{
    VetPayload milkingVet;
    MilkingPayload milking;
    uint8_t raw[64];
};

// الهيكل النهائي للحزمة الموحدة
struct SmartPacket
{
    PacketHeader header;
    PacketPayload payload;
};

#pragma pack(pop)

// التأكد في وقت الـ Compile أن الحزمة لا تتعدى الحد الأقصى لـ ESP-NOW (250 Bytes)
static_assert(sizeof(SmartPacket) <= 250, "SmartPacket exceeds ESP-NOW maximum payload size!");

#endif // PACKET_PROTOCOL_H
