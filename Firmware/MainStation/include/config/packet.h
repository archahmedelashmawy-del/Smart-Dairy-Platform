#pragma once

#include <Arduino.h>

enum class DeviceType : uint8_t
{
    MAIN_STATION = 1,
    PORTABLE_VET = 2,
    DESKTOP_GATEWAY = 3
};

enum class PacketType : uint8_t
{
    HEARTBEAT = 1,

    MILKING_START,

    MILKING_UPDATE,

    MILKING_FINISH,

    VET_EVENT,

    DEVICE_STATUS,

    ACK,

    ERROR
};

struct PacketHeader
{
    uint16_t protocolVersion;

    uint32_t packetID;

    uint32_t sessionID;

    uint32_t timestamp;

    DeviceType source;

    PacketType type;

    uint16_t payloadLength;
};

struct VetPayload
{
    uint32_t cowID;

    bool blocked;

    char category[20];

    char reason[40];
};

struct MilkingPayload
{
    uint32_t cowID;

    float liters;

    float weight;

    float milkTemperature;

    uint16_t suction;

    bool running;
};

struct SmartPacket
{
    PacketHeader header;

    union
    {
        VetPayload vet;

        MilkingPayload milking;
    };
};
