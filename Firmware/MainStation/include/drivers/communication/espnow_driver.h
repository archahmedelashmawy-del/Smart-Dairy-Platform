#ifndef ESPNOW_DRIVER_H
#define ESPNOW_DRIVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "core/error_codes.h"
#include "config/packet.h"

/*
==========================================================
 Smart Dairy Platform
 Decoupled Container Structure (SDP-ENG-0022)
 Allows upper layers to consume packets without ESP-NOW API dependency
==========================================================
*/
struct ReceivedPacket
{
    SmartPacket packet;

    uint8_t senderMAC[6];

    int8_t rssi;

    uint32_t receivedAt;
};

/*
==========================================================
 Communication Statistics Container
 Tracking operational metrics for diagnostics and quality
==========================================================
*/
struct CommunicationStatistics
{
    uint32_t packetsSent;
    uint32_t packetsReceived;
    uint32_t packetsDropped;
    uint32_t invalidPackets;
    uint32_t sendFailures;
    uint32_t peerAddFailures;
    uint32_t peerRemoveFailures;
};

class ESPNowDriver
{
public:

    using ReceiveCallback =
        void (*)(const ReceivedPacket&);

    // Using bool success decouples upper layers from esp_now_send_status_t
    using SendCallback =
        void (*)(const uint8_t* mac, bool success);

public:

    ESPNowDriver();

    ~ESPNowDriver();

    ErrorCode begin();

    void end();

    bool isInitialized() const;

    ErrorCode addPeer(const uint8_t* mac);

    ErrorCode removePeer(const uint8_t* mac);

    ErrorCode sendPacket(
            const uint8_t* mac,
            const SmartPacket& packet);

    ErrorCode broadcast(
            const SmartPacket& packet);

    void registerReceiveCallback(
            ReceiveCallback callback);

    void registerSendCallback(
            SendCallback callback);

    uint8_t peerCount() const;

    void resetStatistics();

    const CommunicationStatistics& getStatistics() const;

    void printStatistics(Stream& stream) const;

private:

    static void onReceive(
        const esp_now_recv_info* info,
        const uint8_t* data,
        int len);

    static void onSend(
        const uint8_t* mac,
        esp_now_send_status_t status);

    void handleReceive(
        const esp_now_recv_info* info,
        const uint8_t* data,
        int len);

    void handleSend(
        const uint8_t* mac,
        esp_now_send_status_t status);

    static ErrorCode validatePacket(
        const SmartPacket& packet,
        int len);

private:

    static ESPNowDriver* instance;

    bool initialized;

    uint8_t peers;

    CommunicationStatistics statistics;

    static ReceiveCallback receiveCallback;

    static SendCallback sendCallback;
};

#endif
