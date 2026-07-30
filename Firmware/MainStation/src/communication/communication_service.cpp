#include "services/communication/communication_service.h"

#include "core/logger.h"

namespace
{
    constexpr uint32_t INVALID_TIMESTAMP = 0;
}

/*----------------------------------------------------------
    Constructor
----------------------------------------------------------*/

CommunicationService::CommunicationService(
    ESPNowDriver& espDriver)
    :
    driver(espDriver),
    head(0),
    tail(0),
    count(0),
    initialized(false)
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

    driver.setReceiveCallback(
        [this](const ReceivedPacket& packet)
        {
            enqueue(packet);
        });

    initialized = true;

    Logger::info("CommunicationService initialized");

    return ErrorCode::OK;
}

/*----------------------------------------------------------
    Update
----------------------------------------------------------*/

void CommunicationService::update()
{
    if (!initialized)
        return;

    ReceivedPacket packet;

    while (dequeue(packet))
    {
        processIncomingPacket(packet);
    }
}

/*----------------------------------------------------------
    Queue
----------------------------------------------------------*/

bool CommunicationService::enqueue(
    const ReceivedPacket& packet)
{
    if (count >= SystemConstants::COMMUNICATION_QUEUE_SIZE)
    {
        Logger::warning("Communication queue full");

        return false;
    }

    queue[tail] = packet;

    tail++;

    if (tail >= SystemConstants::COMMUNICATION_QUEUE_SIZE)
        tail = 0;

    count++;

    return true;
}

bool CommunicationService::dequeue(
    ReceivedPacket& packet)
{
    if (count == 0)
        return false;

    packet = queue[head];

    head++;

    if (head >= SystemConstants::COMMUNICATION_QUEUE_SIZE)
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
    Packet Processing
----------------------------------------------------------*/

void CommunicationService::processIncomingPacket(
    const ReceivedPacket& packet)
{
    publishCommunicationEvent(
        EventType::PacketReceived,
        packet);
}

/*----------------------------------------------------------
    Event Publishing
----------------------------------------------------------*/

void CommunicationService::publishCommunicationEvent(
    EventType type,
    const ReceivedPacket& packet)
{
    SystemEvent event;

    event.type = type;

    event.timestamp =
        (packet.timestamp != INVALID_TIMESTAMP)
        ? packet.timestamp
        : millis();

    event.source = EventSource::Communication;

    event.payload = &packet;

    event.payloadSize = sizeof(ReceivedPacket);

    EventBus::publish(event);
}
