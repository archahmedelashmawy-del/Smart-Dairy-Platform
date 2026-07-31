#include "services/communication/communication_service.h"

#include "core/logger.h"

namespace
{
    constexpr uint32_t INVALID_TIMESTAMP = 0;
}

/*----------------------------------------------------------
    Static Bridge Handler
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
    Constructor (Runtime Guard - Blocking 1)
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
    // Runtime Guard against multiple service instantiation
    if (instance != nullptr)
    {
        Logger::error("CommunicationService already instantiated!");
        abort();
    }

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

    CommunicationEvent currentEvent;

    while (dequeue(currentEvent))
    {
        processIncomingPacket(currentEvent);
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
    Queue Management
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

    // CRITICAL MEMORY SAFETY NOTE:
    // payload points to a stack object during dispatch.
    // EventBus subscribers MUST consume it immediately synchronously.
    // Do NOT retain this pointer for deferred processing.
    sysEvent.payload = &event;

    sysEvent.payloadSize = sizeof(CommunicationEvent);

    EventBus::publish(sysEvent);
}
