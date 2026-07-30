#include "services/communication/communication_service.h"
#include "core/logger.h"

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

    // تسجيل Callback الاستقبال وتضمين بيانات الـ Metadata
    driver.registerReceiveCallback([this](const SystemPacket& packet) {
        ReceivedPacket rxPacket;
        rxPacket.packet = packet;
        rxPacket.receivedAt = millis();
        rxPacket.rssi = 0; // يتم تحديثه لاحقاً إن أتيح من الـ Driver
        memset(rxPacket.senderMac, 0, sizeof(rxPacket.senderMac));

        enqueue(rxPacket);
    });

    initialized = true;
    Logger::info("CommunicationService initialized successfully.");

    return ErrorCode::OK;
}

/*----------------------------------------------------------
    Queue Management
----------------------------------------------------------*/

bool CommunicationService::enqueue(const ReceivedPacket& rxPacket)
{
    if (count >= RX_QUEUE_SIZE)
    {
        droppedPackets++;
        Logger::error("Communication RX queue overflow. Packet dropped.");
        return false;
    }

    rxQueue[tail] = rxPacket;
    tail = (tail + 1) % RX_QUEUE_SIZE;
    count++;

    return true;
}

bool CommunicationService::dequeue(ReceivedPacket& rxPacket)
{
    if (count == 0)
        return false;

    rxPacket = rxQueue[head];
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
    Event Publishing Wrapper
----------------------------------------------------------*/

void CommunicationService::publishEvent(EventType type, const SystemPacket& packet)
{
    Event event;
    event.type = type;
    event.timestamp = millis();
    event.packet = packet;

    EventBus::publish(event);
}

/*----------------------------------------------------------
    Update Loop
----------------------------------------------------------*/

void CommunicationService::update()
{
    if (!initialized)
        return;

    ReceivedPacket rxPacket;

    while (dequeue(rxPacket))
    {
        processIncomingPacket(rxPacket);
    }
}
