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

    ~CommunicationService();

    [[nodiscard]] ErrorCode begin();

    void update();

    [[nodiscard]] ErrorCode sendPacket(
        const SmartPacket& packet,
        const uint8_t* peer);

    [[nodiscard]] ErrorCode broadcast(
        const SmartPacket& packet);

    [[nodiscard]] bool hasConnection() const;

    [[nodiscard]] uint32_t queueOverflowCount() const;

private:

    static void onDataReceived(const ReceivedPacket& rxPacket);

    [[nodiscard]] bool enqueue(const CommunicationEvent& event);

    [[nodiscard]] bool dequeue(CommunicationEvent& event);

    void clearQueue();

    [[nodiscard]] size_t pendingPackets() const;

    void processIncomingPacket(
        const CommunicationEvent& event);

    void publishCommunicationEvent(
        EventType type,
        const CommunicationEvent& event);

private:

    static CommunicationService* instance;

    ESPNowDriver& driver;

    CommunicationEvent queue[SystemConstants::COMMUNICATION_QUEUE_SIZE];

    size_t head;

    size_t tail;

    size_t count;

    uint32_t overflows;

    bool initialized;
};

#endif
