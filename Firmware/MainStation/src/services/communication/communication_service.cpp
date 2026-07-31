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

    driver.registerReceiveCallback(
        [this](const ReceivedPacket& rxPacket)
        {
            CommunicationEvent commEvent;
            commEvent.packet = rxPacket;
            enqueue(commEvent);
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

    CommunicationEvent event;

    while (dequeue(event))
    {
        processIncomingPacket(event);
    }
}

/*----------------------------------------------------------
    Send & Broadcast API
----------------------------------------------------------*/

ErrorCode CommunicationService::sendPacket(
    const SmartPacket& packet,
    const uint8_t* peer)
{
    if (!initialized)
        return ErrorCode::ESPNOW_NOT_INITIALIZED;

    return driver.sendPacket(peer, packet);
}

ErrorCode CommunicationService::broadcast(
    const SmartPacket& packet)
{
    if (!initialized)
        return ErrorCode::ESPNOW_NOT_INITIALIZED;

    return driver.broadcast(packet);
}

bool CommunicationService::hasConnection() const
{
    return initialized && (driver.peerCount() > 0);
}

/*----------------------------------------------------------
    Queue
----------------------------------------------------------*/

bool CommunicationService::enqueue(
    const CommunicationEvent& event)
{
    if (count >= SystemConstants::COMMUNICATION_QUEUE_SIZE)
    {
        Logger::warning("Communication queue full");

        return false;
    }

    queue[tail] = event;

    tail++;

    if (tail >= SystemConstants::COMMUNICATION_QUEUE_SIZE)
        tail = 0;

    count++;

    return true;
}

bool CommunicationService::dequeue(
    CommunicationEvent& event)
{
    if (count == 0)
        return false;

    event = queue[head];

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
    const CommunicationEvent& event)
{
    publishCommunicationEvent(
        EventType::PacketReceived,
        event);
}

/*----------------------------------------------------------
    Event Publishing
----------------------------------------------------------*/

void CommunicationService::publishCommunicationEvent(
    EventType type,
    const CommunicationEvent& event)
{
    SystemEvent sysEvent;

    sysEvent.type = type;

    sysEvent.timestamp =
        (event.packet.receivedAt != INVALID_TIMESTAMP)
        ? event.packet.receivedAt
        : millis();

    sysEvent.source = EventSource::Communication;

    // NOTE: payload pointer is valid only synchronously during publish().
    // If EventBus switches to an asynchronous queue in the future, payload must be deep-copied.
    sysEvent.payload = &event;

    sysEvent.payloadSize = sizeof(CommunicationEvent);

    EventBus::publish(sysEvent);
}
