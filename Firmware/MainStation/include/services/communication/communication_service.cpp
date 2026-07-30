#include "services/communication/communication_service.h"
#include "core/logger.h"
#include "core/event_bus.h"

namespace
{
    constexpr size_t RX_QUEUE_SIZE = 16;
}

/*----------------------------------------------------------
    Constructor
----------------------------------------------------------*/

CommunicationService::CommunicationService(ESPNowDriver& espNowDriver)
    : driver(espNowDriver),
      initialized(false),
      head(0),
      tail(0),
      count(0)
{
}

/*----------------------------------------------------------
    Initialization
----------------------------------------------------------*/

ErrorCode CommunicationService::begin()
{
    if (initialized)
        return ErrorCode::OK;

    ErrorCode err = driver.begin();
    if (err != ErrorCode::OK)
        return err;

    driver.setReceiveCallback([this](const SystemPacket& packet) {
        enqueue(packet);
    });

    initialized = true;
    Logger::info("CommunicationService initialized.");

    return ErrorCode::OK;
}

/*----------------------------------------------------------
    Queue Management
----------------------------------------------------------*/

bool CommunicationService::enqueue(const SystemPacket& packet)
{
    if (count >= RX_QUEUE_SIZE)
    {
        Logger::warning("Communication queue full.");
        return false;
    }

    rxQueue[tail] = packet;
    tail++;

    if (tail >= RX_QUEUE_SIZE)
        tail = 0;

    count++;
    return true;
}

bool CommunicationService::dequeue(SystemPacket& packet)
{
    if (count == 0)
        return false;

    packet = rxQueue[head];
    head++;

    if (head >= RX_QUEUE_SIZE)
        head = 0;

    count--;
    return true;
}

void CommunicationService::clearQueue()
{
    head = 0;
    tail = 0;
    count = 0;
}

size_t CommunicationService::pendingPackets() const
{
    return count;
}

/*----------------------------------------------------------
    Update Loop
----------------------------------------------------------*/

void CommunicationService::update()
{
    if (!initialized)
        return;

    SystemPacket packet;

    while (dequeue(packet))
    {
        processIncomingPacket(packet);
    }
}
