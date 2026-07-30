#include "services/communication/communication_service.h"
#include "core/logger.h"
#include "core/event_bus.h"

/*----------------------------------------------------------
    Constructor
----------------------------------------------------------*/

CommunicationService::CommunicationService(ESPNowDriver& driver)
    : driver(driver),
      initialized(false),
      head(0),
      tail(0),
      count(0),
      droppedPackets(0)
{
}

/*----------------------------------------------------------
    Initialization
----------------------------------------------------------*/

ErrorCode CommunicationService::begin()
{
    if (initialized)
        return ErrorCode::OK;

    clearQueue();
    droppedPackets = 0;

    ErrorCode err = driver.begin();
    if (err != ErrorCode::OK)
        return err;

    driver.registerReceiveCallback([this](const SystemPacket& packet) {
        enqueue(packet);
    });

    initialized = true;
    Logger::info("CommunicationService initialized successfully.");

    return ErrorCode::OK;
}

/*----------------------------------------------------------
    Queue Management
----------------------------------------------------------*/

bool CommunicationService::enqueue(const SystemPacket& packet)
{
    if (count >= RX_QUEUE_SIZE)
    {
        droppedPackets++;
        Logger::error("Communication RX queue overflow. Packet dropped.");
        return false;
    }

    rxQueue[tail] = packet;
    tail = (tail + 1) % RX_QUEUE_SIZE;
    count++;

    return true;
}

bool CommunicationService::dequeue(SystemPacket& packet)
{
    if (count == 0)
        return false;

    packet = rxQueue[head];
    head = (head + 1) % RX_QUEUE_SIZE;
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

size_t CommunicationService::getDroppedPackets() const
{
    return droppedPackets;
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
