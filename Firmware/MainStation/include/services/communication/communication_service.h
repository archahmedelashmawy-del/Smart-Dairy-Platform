#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <Arduino.h>
#include "core/error_codes.h"
#include "config/packet.h"
#include "drivers/communication/espnow_driver.h"

class CommunicationService
{
public:
    explicit CommunicationService(ESPNowDriver& driver);
    ~CommunicationService() = default;

    ErrorCode begin();
    void update();

    void clearQueue();
    size_t pendingPackets() const;
    size_t getDroppedPackets() const;

private:
    bool enqueue(const SystemPacket& packet);
    bool dequeue(SystemPacket& packet);
    void processIncomingPacket(const SystemPacket& packet);

private:
    ESPNowDriver& driver;
    bool initialized;

    static constexpr size_t RX_QUEUE_SIZE = 16;
    SystemPacket rxQueue[RX_QUEUE_SIZE];

    size_t head;
    size_t tail;
    size_t count;
    size_t droppedPackets;
};

#endif
