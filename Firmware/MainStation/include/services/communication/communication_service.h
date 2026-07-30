#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <Arduino.h>

#include "core/error_codes.h"
#include "core/event.h"
#include "core/event_bus.h"

#include "config/packet.h"

#include "drivers/communication/espnow_driver.h"

constexpr size_t COMM_QUEUE_SIZE = 16;

struct ReceivedPacket
{
    ESPNowPacket packet;

    uint8_t senderMac[6];

    int8_t rssi;

    uint32_t timestamp;
};

class CommunicationService
{
public:

    explicit CommunicationService(ESPNowDriver& driver);

    ErrorCode begin();

    void update();

    ErrorCode sendPacket(
        const ESPNowPacket& packet,
        const uint8_t* peer);

    ErrorCode broadcast(
        const ESPNowPacket& packet);

    bool hasConnection() const;

private:

    bool enqueue(const ReceivedPacket& packet);

    bool dequeue(ReceivedPacket& packet);

    void clearQueue();

    size_t pendingPackets() const;

    void processIncomingPacket(
        const ReceivedPacket& packet);

    void publishCommunicationEvent(
        EventType type,
        const ReceivedPacket& packet);

private:

    ESPNowDriver& driver;

    ReceivedPacket queue[COMM_QUEUE_SIZE];

    size_t head;

    size_t tail;

    size_t count;

    bool initialized;
};

#endif
