#pragma once

#include <stdint.h>

enum class ErrorCode : uint16_t
{
    NONE = 0,

    // System
    SYSTEM_BOOT_FAILED,
    WATCHDOG_RESET,

    // Storage
    SD_INIT_FAILED,
    SD_WRITE_FAILED,
    SD_READ_FAILED,

    // RFID
    RFID_NOT_FOUND,
    RFID_READ_FAILED,

    // Temperature
    TEMP_SENSOR_ERROR,

    // RTC
    RTC_NOT_FOUND,
    RTC_INVALID_TIME,

    // Pump
    PUMP_START_TIMEOUT,
    PUMP_STOP_TIMEOUT,

    // Servo
    SERVO_ERROR,

    // ESP-NOW
    ESPNOW_INIT_FAILED,
    ESPNOW_SEND_FAILED,
    ESPNOW_TIMEOUT,
    ESPNOW_INVALID_PACKET,

    // WiFi
    WIFI_DISCONNECTED,

    // Database Sync
    SYNC_FAILED,

    UNKNOWN
};
