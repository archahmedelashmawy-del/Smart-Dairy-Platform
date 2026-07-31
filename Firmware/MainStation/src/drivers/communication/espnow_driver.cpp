#include "drivers/communication/espnow_driver.h"

#include <WiFi.h>
#include <esp_now.h>
#include "core/logger.h"

//====================================================
// Static Member Initialization (Blocking 1 - Resolved)
//====================================================

ESPNowDriver* ESPNowDriver::instance = nullptr;
ESPNowDriver::ReceiveCallback ESPNowDriver::receiveCallback = nullptr;
ESPNowDriver::SendCallback ESPNowDriver::sendCallback = nullptr;

//====================================================
// Constructor & Destructor (Blocking 7 - Life Cycle)
//====================================================

ESPNowDriver::ESPNowDriver()
    : initialized(false), peers(0)
{
    if (instance != nullptr)
    {
        Logger::error("ESPNowDriver already instantiated! Single instance required.");
        abort();
    }

    memset(&statistics, 0, sizeof(statistics));
    instance = this;
}

ESPNowDriver::~ESPNowDriver()
{
    end();
    instance = nullptr;
}

//====================================================
// Internal Callback Bridges (Receiving & Sending - Blocking 2)
//====================================================

void ESPNowDriver::onReceiveInternal(
    const esp_now_recv_info* info,
    const uint8_t* data,
    int len)
{
    if (instance == nullptr)
        return;

    instance->handleReceive(info, data, len);
}

void ESPNowDriver::onSendInternal(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    if (instance == nullptr)
        return;

    instance->handleSend(mac_addr, status);
}

//====================================================
// Internal Handlers (Blocking 3 - Accurate Tx Stats)
//====================================================

void ESPNowDriver::handleReceive(
    const esp_now_recv_info* info,
    const uint8_t* data,
    int len)
{
    statistics.packetsReceived++;

    if (data == nullptr || info == nullptr)
    {
        statistics.packetsDropped++;
        return;
    }

    if (len != sizeof(SmartPacket))
    {
        statistics.invalidPackets++;
        statistics.packetsDropped++;
        return;
    }

    SmartPacket packet;
    memcpy(&packet, data, sizeof(packet));

    ErrorCode result = validatePacket(packet, len);

    if (result != ErrorCode::OK)
    {
        statistics.invalidPackets++;
        statistics.packetsDropped++;
        return;
    }

    // Architectural Note (Blocking 4):
    // Registered receiveCallbacks MUST remain non-blocking (O(1) queue insertion only)
    // as execution context originates from ESP-IDF WiFi task stack.
    if (receiveCallback != nullptr)
    {
        ReceivedPacket rxContainer;
        rxContainer.packet = packet;
        
        if (info->src_addr != nullptr)
        {
            memcpy(rxContainer.senderMAC, info->src_addr, ESP_NOW_ETH_ALEN);
        }
        else
        {
            memset(rxContainer.senderMAC, 0, ESP_NOW_ETH_ALEN);
        }

        rxContainer.rssi = (info->rx_ctrl != nullptr) ? info->rx_ctrl->rssi : 0;
        rxContainer.receivedAt = millis();

        receiveCallback(rxContainer);
    }
}

void ESPNowDriver::handleSend(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    bool success = (status == ESP_NOW_SEND_SUCCESS);

    if (success)
    {
        statistics.packetsSent++;
    }
    else
    {
        statistics.sendFailures++;
    }

    if (sendCallback != nullptr)
    {
        sendCallback(mac_addr, success);
    }
}

//====================================================
// Packet Validation
//====================================================

ErrorCode ESPNowDriver::validatePacket(const SmartPacket& packet, int len)
{
    if (len != sizeof(SmartPacket))
    {
        return ErrorCode::INVALID_PACKET_SIZE;
    }

    if (packet.header.payloadLength > sizeof(SmartPacket))
    {
        return ErrorCode::INVALID_PAYLOAD_SIZE;
    }

    if (packet.header.protocolVersion != PROTOCOL_VERSION)
    {
        return ErrorCode::INVALID_PROTOCOL_VERSION;
    }

    switch (packet.header.source)
    {
        case DeviceType::MAIN_STATION:
        case DeviceType::PORTABLE_VET:
        case DeviceType::DESKTOP_GATEWAY:
            break;

        default:
            return ErrorCode::INVALID_DEVICE_TYPE;
    }

    if (packet.header.packetID == 0)
    {
        return ErrorCode::INVALID_PACKET_ID;
    }

    if (packet.header.sessionID == 0)
    {
        return ErrorCode::INVALID_SESSION_ID;
    }

    switch (packet.header.type)
    {
        case PacketType::HEARTBEAT:
        case PacketType::ACK:
        case PacketType::ERROR:
        case PacketType::DEVICE_STATUS:
            if (packet.header.payloadLength != 0)
            {
                return ErrorCode::INVALID_PAYLOAD_SIZE;
            }
            break;

        case PacketType::VET_EVENT:
            if (packet.header.payloadLength != sizeof(VetPayload))
            {
                return ErrorCode::INVALID_PAYLOAD_SIZE;
            }
            break;

        case PacketType::MILKING_START:
        case PacketType::MILKING_UPDATE:
        case PacketType::MILKING_FINISH:
            if (packet.header.payloadLength != sizeof(MilkingPayload))
            {
                return ErrorCode::INVALID_PAYLOAD_SIZE;
            }
            break;

        default:
            return ErrorCode::INVALID_PACKET_TYPE;
    }

    if (packet.header.timestamp == 0)
    {
        return ErrorCode::INVALID_TIMESTAMP;
    }

    return ErrorCode::OK;
}

//====================================================
// Initialization & Deinitialization (Blocking 2 & 7)
//====================================================

ErrorCode ESPNowDriver::begin()
{
    if (initialized)
        return ErrorCode::OK;

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        return ErrorCode::ESPNOW_INIT_FAILED;
    }

    // Register Both Callbacks
    esp_now_register_recv_cb(onReceiveInternal);
    esp_now_register_send_cb(onSendInternal);

    initialized = true;

    return ErrorCode::OK;
}

void ESPNowDriver::end()
{
    if (!initialized)
        return;

    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    esp_now_deinit();

    initialized = false;
}

bool ESPNowDriver::isInitialized() const
{
    return initialized;
}

//====================================================
// Callback Registration
//====================================================

void ESPNowDriver::registerReceiveCallback(ReceiveCallback callback)
{
    receiveCallback = callback;
}

void ESPNowDriver::registerSendCallback(SendCallback callback)
{
    sendCallback = callback;
}

//====================================================
// Peer Management
//====================================================

ErrorCode ESPNowDriver::addPeer(const uint8_t* mac)
{
    if (!initialized)
        return ErrorCode::ESPNOW_NOT_INITIALIZED;

    if (mac == nullptr)
        return ErrorCode::INVALID_MAC_ADDRESS;

    if (esp_now_is_peer_exist(mac))
        return ErrorCode::OK;

    esp_now_peer_info_t peer{};
    memcpy(peer.peer_addr, mac, ESP_NOW_ETH_ALEN);
    peer.channel = 0;
    peer.encrypt = false;

    if (esp_now_add_peer(&peer) != ESP_OK)
    {
        statistics.peerAddFailures++;
        return ErrorCode::PEER_ADD_FAILED;
    }

    peers++;
    return ErrorCode::OK;
}

ErrorCode ESPNowDriver::removePeer(const uint8_t* mac)
{
    if (!initialized)
        return ErrorCode::ESPNOW_NOT_INITIALIZED;

    if (mac == nullptr)
        return ErrorCode::INVALID_MAC_ADDRESS;

    if (!esp_now_is_peer_exist(mac))
        return ErrorCode::OK;

    if (esp_now_del_peer(mac) != ESP_OK)
    {
        statistics.peerRemoveFailures++;
        return ErrorCode::PEER_REMOVE_FAILED;
    }

    if (peers > 0) peers--;
    return ErrorCode::OK;
}

uint8_t ESPNowDriver::peerCount() const
{
    return peers;
}

//====================================================
// Send / Broadcast API
//====================================================

ErrorCode ESPNowDriver::sendPacket(const uint8_t* mac, const SmartPacket& packet)
{
    if (!initialized)
        return ErrorCode::ESPNOW_NOT_INITIALIZED;

    if (mac == nullptr)
        return ErrorCode::INVALID_MAC_ADDRESS;

    ErrorCode validation = validatePacket(packet, sizeof(packet));

    if (validation != ErrorCode::OK)
    {
        statistics.invalidPackets++;
        return validation;
    }

    esp_err_t result = esp_now_send(
        mac,
        reinterpret_cast<const uint8_t*>(&packet),
        sizeof(packet)
    );

    if (result != ESP_OK)
    {
        statistics.sendFailures++;
        return ErrorCode::SEND_FAILED;
    }

    return ErrorCode::OK;
}

ErrorCode ESPNowDriver::broadcast(const SmartPacket& packet)
{
    static constexpr uint8_t BROADCAST_MAC[ESP_NOW_ETH_ALEN] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    return sendPacket(BROADCAST_MAC, packet);
}

//====================================================
// Diagnostics & Statistics
//====================================================

void ESPNowDriver::resetStatistics()
{
    memset(&statistics, 0, sizeof(statistics));
}

const CommunicationStatistics& ESPNowDriver::getStatistics() const
{
    return statistics;
}

void ESPNowDriver::printStatistics(Stream& stream) const
{
    stream.println(F("ESP-NOW Communication Statistics"));
    stream.println(F("--------------------------------"));
    stream.print(F("Packets Sent        : ")); stream.println(statistics.packetsSent);
    stream.print(F("Packets Received    : ")); stream.println(statistics.packetsReceived);
    stream.print(F("Packets Dropped     : ")); stream.println(statistics.packetsDropped);
    stream.print(F("Invalid Packets     : ")); stream.println(statistics.invalidPackets);
    stream.print(F("Send Failures       : ")); stream.println(statistics.sendFailures);
    stream.print(F("Peer Add Failures   : ")); stream.println(statistics.peerAddFailures);
    stream.print(F("Peer Remove Failures: ")); stream.println(statistics.peerRemoveFailures);
}
