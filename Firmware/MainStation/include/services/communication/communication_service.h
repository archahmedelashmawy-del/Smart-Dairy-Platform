#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <Arduino.h>

#include "core/error_codes.h"
#include "core/event.h"
#include "core/event_bus.h"

#include "config/constants.h"
#include "config/packet.h"

#include "drivers/communication/espnow_driver.h"
#include "models/communication_event.h"

class CommunicationService
{
public:

    explicit CommunicationService(ESPNowDriver& driver);

    [[nodiscard]] ErrorCode begin();

    void update();

    [[nodiscard]] ErrorCode sendPacket(
        const SmartPacket& packet,
        const uint8_t* peer);

    [[nodiscard]] ErrorCode broadcast(
        const SmartPacket& packet);

    [[nodiscard]] bool hasConnection() const;

private:

    [[nodiscard]] bool enqueue(const ReceivedPacket& packet);

    [[nodiscard]] bool dequeue(ReceivedPacket& packet);

    void clearQueue();

    [[nodiscard]] size_t pendingPackets() const;

    void processIncomingPacket(
        const ReceivedPacket& packet);

    void publishCommunicationEvent(
        EventType type,
        const ReceivedPacket& packet);

private:

    ESPNowDriver& driver;

    ReceivedPacket queue[SystemConstants::COMMUNICATION_QUEUE_SIZE];

    size_t head;

    size_t tail;

    size_t count;

    bool initialized;
};

#endif
