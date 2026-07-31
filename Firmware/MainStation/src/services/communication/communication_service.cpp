#include "services/communication/communication_service.h"

#include "core/logger.h"

namespace
{
    constexpr uint32_t INVALID_TIMESTAMP = 0;
}

/*----------------------------------------------------------
    Static Bridge Handler (Blocking 4 Solution)
----------------------------------------------------------*/

void CommunicationService::onDataReceived(const ReceivedPacket& rxPacket)
{
    if (instance == nullptr)
        return;

    CommunicationEvent commEvent;
    commEvent.packet = rxPacket;
    instance->enqueue(commEvent);
}

// Static Instance Initialization
CommunicationService* CommunicationService::instance = nullptr;

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
    instance = this;
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

    // Register static callback function pointer (Avoids Lambda-capturing issue)
    driver.registerReceiveCallback(CommunicationService::onDataReceived);

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
    Queue Management (Improvement 1 Applied)
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
    tail = (tail + 1) % SystemConstants::COMMUNICATION_QUEUE_SIZE;
    count++;

    return true;
}

bool CommunicationService::dequeue(
    CommunicationEvent& event)
{
    if (count == 0)
        return false;

    event = queue[head];
    head = (head + 1) % SystemConstants::COMMUNICATION_QUEUE_SIZE;
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
    Event Publishing (Blocking 3 Applied)
----------------------------------------------------------*/

void CommunicationService::publishCommunicationEvent(
    EventType type,
    const CommunicationEvent& event)
{
    SystemEvent sysEvent;

    sysEvent.type = type;

    // Preserve local receive timestamp
    sysEvent.timestamp =
        (event.packet.receivedAt != INVALID_TIMESTAMP)
        ? event.packet.receivedAt
        : millis();

    sysEvent.source = EventSource::Communication;

    // CRITICAL MEMORY SAFETY NOTE (Blocking 3):
    // payload points to a stack object.
    // EventBus subscribers MUST consume it immediately synchronously.
    // Do NOT store this pointer for asynchronous processing.
    sysEvent.payload = &event;

    sysEvent.payloadSize = sizeof(CommunicationEvent);

    EventBus::publish(sysEvent);
}
