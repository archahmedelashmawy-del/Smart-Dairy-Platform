#pragma once

namespace SystemConstants {

//========================
// Timing
//========================

constexpr uint32_t LOOP_INTERVAL_MS = 50;

constexpr uint32_t LCD_REFRESH_MS = 250;

constexpr uint32_t SENSOR_REFRESH_MS = 500;

constexpr uint32_t WIFI_CHECK_MS = 5000;

constexpr uint32_t ESP_NOW_TIMEOUT_MS = 1500;

constexpr uint32_t PUMP_START_DELAY_MS = 5000;

constexpr uint32_t SERVO_RETRACT_DELAY_MS = 3000;

constexpr uint32_t SESSION_STOP_DELAY_MS = 5000;

//========================
// Suction
//========================

constexpr uint16_t SUCTION_THRESHOLD = 300;

constexpr float FLOW_FACTOR = 1500.0f;

//========================
// Milk
//========================

constexpr float DEFAULT_MILK_TEMP = 38.2f;

constexpr float WEIGHT_FACTOR = 1.03f;

//========================
// Retry
//========================

constexpr uint8_t MAX_PACKET_RETRY = 3;

constexpr uint8_t MAX_SEND_QUEUE = 32;

//========================
// Storage
//========================

constexpr uint16_t MAX_LOCAL_EVENTS = 1000;

constexpr uint16_t MAX_SESSION_RECORDS = 5000;

//========================
// Device
//========================

constexpr uint16_t DEVICE_HEARTBEAT_SEC = 30;

constexpr uint16_t WATCHDOG_TIMEOUT_SEC = 10;

}
