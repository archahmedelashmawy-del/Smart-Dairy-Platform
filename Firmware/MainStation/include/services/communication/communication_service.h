#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <Arduino.h>
#include "core/error_codes.h"
#include "config/packet.h"
#include "drivers/communication/espnow_driver.h"

// تحديد حجم الـ Queue (Circular Buffer)
constexpr size_t COMM_QUEUE_SIZE = 16;

class CommunicationService
{
public:
    CommunicationService(ESPNowDriver& driver);

    ErrorCode begin();
    void update();

    ErrorCode sendPacket(const ESPNowPacket& packet, const uint8_t* peer);
    ErrorCode broadcast(const ESPNowPacket& packet);

    bool hasConnection() const;

private:
    void processIncomingPacket(const ESPNowPacket& packet);
    void processOutgoingStatus();

    // Circular Queue Management
    bool pushToQueue(const ESPNowPacket& packet);
    bool popFromQueue(ESPNowPacket& packet);

private:
    ESPNowDriver& espDriver;

    // Circular Buffer variables
    ESPNowPacket packetQueue[COMM_QUEUE_SIZE];
    size_t head;
    size_t tail;
    size_t queueCount;

    bool initialized;
};

#endif
