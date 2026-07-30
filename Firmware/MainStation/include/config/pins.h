#pragma once

namespace Pins {

//=========================
// SPI BUS
//=========================

constexpr uint8_t SPI_SCK  = 18;
constexpr uint8_t SPI_MISO = 19;
constexpr uint8_t SPI_MOSI = 23;

//=========================
// RFID
//=========================

constexpr uint8_t RFID_SS  = 5;
constexpr uint8_t RFID_RST = 17;

//=========================
// I2C
//=========================

constexpr uint8_t I2C_SDA = 21;
constexpr uint8_t I2C_SCL = 22;

//=========================
// MAIN STATION
//=========================

constexpr uint8_t SERVO_PIN      = 4;
constexpr uint8_t RELAY_PIN      = 16;
constexpr uint8_t BUZZER_PIN     = 15;
constexpr uint8_t SD_CS_PIN      = 13;
constexpr uint8_t TEMP_PIN       = 2;
constexpr uint8_t SUCTION_PIN    = 34;

//=========================
// KEYPAD
//=========================

constexpr uint8_t ROW1 = 35;
constexpr uint8_t ROW2 = 12;
constexpr uint8_t ROW3 = 14;
constexpr uint8_t ROW4 = 27;

constexpr uint8_t COL1 = 26;
constexpr uint8_t COL2 = 25;
constexpr uint8_t COL3 = 33;
constexpr uint8_t COL4 = 32;

//=========================
// PORTABLE DEVICE BUTTONS
//=========================

constexpr uint8_t BTN_UP     = 34;
constexpr uint8_t BTN_DOWN   = 35;
constexpr uint8_t BTN_SELECT = 32;
constexpr uint8_t BTN_BACK   = 33;

}
