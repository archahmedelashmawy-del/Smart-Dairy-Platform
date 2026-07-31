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

class ESPNowDriver
{
public:

    using ReceiveCallback =
        void (*)(const ReceivedPacket&);

    using SendCallback =
        void (*)(const uint8_t* mac,
                 esp_now_send_status_t status);

public:

    ESPNowDriver();

    ErrorCode begin();

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

private:

    static void onReceive(
        const esp_now_recv_info* info,
        const uint8_t* data,
        int len);

    static void onSend(
        const uint8_t* mac,
        esp_now_send_status_t status);

    static bool validatePacket(
        const SmartPacket& packet,
        int len);

private:

    bool initialized;

    uint8_t peers;

    static ReceiveCallback receiveCallback;

    static SendCallback sendCallback;
};

#endif
