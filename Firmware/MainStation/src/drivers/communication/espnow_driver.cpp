#include "drivers/communication/espnow_driver.h"

#include <WiFi.h>
#include <esp_now.h>

//====================================================
// Static Member Initialization
//====================================================

ESPNowDriver* ESPNowDriver::instance = nullptr;

//====================================================
// Constructor
//====================================================

ESPNowDriver::ESPNowDriver()
    : initialized(false)
{
    memset(&statistics, 0, sizeof(statistics));
    instance = this;
}

//====================================================
// Internal Receive Callback Bridge
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

//====================================================
// Internal Receive Handler
//====================================================

void ESPNowDriver::handleReceive(
    const esp_now_recv_info* info,
    const uint8_t* data,
    int len)
{
    statistics.packetsReceived++;

    // 1. Basic Validation
    if (data == nullptr)
    {
        statistics.packetsDropped++;
        return;
    }

    if (len != sizeof(ESPNowPacket))
    {
        statistics.invalidPackets++;
        statistics.packetsDropped++;
        return;
    }

    // 2. Copy Packet
    ESPNowPacket packet;
    memcpy(&packet, data, sizeof(packet));

    // 3. Validate Packet
    ErrorCode result = validatePacket(packet);

    if (result != ErrorCode::OK)
    {
        statistics.invalidPackets++;
        statistics.packetsDropped++;
        return;
    }

    // 4. Forward to Upper Layer
    if (receiveCallback != nullptr)
    {
        receiveCallback(packet);
    }
}

//====================================================
// Packet Validation
//====================================================

ErrorCode ESPNowDriver::validatePacket(const ESPNowPacket& packet)
{
    // Protocol Version
    if (packet.protocolVersion != PROTOCOL_VERSION)
    {
        return ErrorCode::INVALID_PROTOCOL_VERSION;
    }

    // Packet Type
    if (packet.packetType >= PacketType::COUNT)
    {
        return ErrorCode::INVALID_PACKET_TYPE;
    }

    // Cow ID (Safe strnlen check)
    if (strnlen(packet.cowID, sizeof(packet.cowID)) == 0)
    {
        return ErrorCode::INVALID_COW_ID;
    }

    // Device ID
    if (packet.deviceID == 0)
    {
        return ErrorCode::INVALID_DEVICE_ID;
    }

    // Timestamp
    if (packet.timestamp == 0)
    {
        return ErrorCode::INVALID_TIMESTAMP;
    }

    return ErrorCode::OK;
}

//====================================================
// Initialization
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

    esp_now_register_recv_cb(onReceiveInternal);

    initialized = true;

    return ErrorCode::OK;
}

bool ESPNowDriver::isInitialized() const
{
    return initialized;
}

//====================================================
// Callback Registration
//====================================================

void ESPNowDriver::registerReceiveCallback(PacketCallback callback)
{
    receiveCallback = callback;
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

    return ErrorCode::OK;
}

//====================================================
// Send / Broadcast API
//====================================================

ErrorCode ESPNowDriver::send(const uint8_t* mac, const ESPNowPacket& packet)
{
    if (!initialized)
        return ErrorCode::ESPNOW_NOT_INITIALIZED;

    if (mac == nullptr)
        return ErrorCode::INVALID_MAC_ADDRESS;

    ErrorCode validation = validatePacket(packet);

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

    statistics.packetsSent++;

    return ErrorCode::OK;
}

ErrorCode ESPNowDriver::broadcast(const ESPNowPacket& packet)
{
    static constexpr uint8_t BROADCAST_MAC[ESP_NOW_ETH_ALEN] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    return send(BROADCAST_MAC, packet);
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
