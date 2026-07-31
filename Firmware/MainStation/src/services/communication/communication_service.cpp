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

    if (!instance->enqueue(commEvent))
    {
        instance->overflows++;
        Logger::warning("Communication queue full! Packet dropped.");
    }
}

// Static Instance Initialization
CommunicationService* CommunicationService::instance = nullptr;

/*----------------------------------------------------------
    Constructor & Destructor
----------------------------------------------------------*/

CommunicationService::CommunicationService(
    ESPNowDriver& espDriver)
    :
    driver(espDriver),
    head(0),
    tail(0),
    count(0),
    overflows(0),
    initialized(false)
{
    if (instance != nullptr)
    {
        Logger::error("CommunicationService already instantiated!");
        abort();
    }

    instance = this;
}

CommunicationService::~CommunicationService()
{
    end();
    instance = nullptr;
}

/*----------------------------------------------------------
    Lifecycle
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

void CommunicationService::end()
{
    if (!initialized)
        return;

    // Unregister only callbacks owned by CommunicationService
    driver.registerReceiveCallback(nullptr);

    clearQueue();
    overflows = 0;
    initialized = false;

    Logger::info("CommunicationService stopped and callbacks cleared.");
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

uint32_t CommunicationService::queueOverflowCount() const
{
    return overflows;
}

void CommunicationService::printDiagnostics(Stream& stream) const
{
    stream.println(F("--- Communication Service Diagnostics ---"));
    stream.print(F("Initialized: "));
    stream.println(initialized ? F("YES") : F("NO"));
    
    stream.print(F("Pending Queue Packets: "));
    stream.println(count);
    
    stream.print(F("Queue Overflow Count: "));
    stream.println(overflows);
    
    driver.printStatistics(stream);
}

/*----------------------------------------------------------
    Queue Management
----------------------------------------------------------*/

bool CommunicationService::enqueue(
    const CommunicationEvent& event)
{
    if (count >= SystemConstants::COMMUNICATION_QUEUE_SIZE)
    {
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
    Packet Processing & Publishing
----------------------------------------------------------*/

void CommunicationService::processIncomingPacket(
    const CommunicationEvent& event)
{
    publishCommunicationEvent(
        EventType::PacketReceived,
        event);
}

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
    // Zero-copy synchronous dispatch.
    // payload points to a stack object during dispatch.
    // EventBus subscribers MUST consume it immediately synchronously.
    // Do NOT retain this pointer for deferred processing.
    sysEvent.payload = &event;

    sysEvent.payloadSize = sizeof(CommunicationEvent);

    EventBus::publish(sysEvent);
}
