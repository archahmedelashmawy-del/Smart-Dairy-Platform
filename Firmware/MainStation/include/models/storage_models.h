#ifndef STORAGE_MODELS_H
#define STORAGE_MODELS_H

#include <Arduino.h>
#include "config/packet.h"

/*
==========================================================
 Milking Session Record Model
==========================================================
*/
struct MilkingRecord
{
    uint32_t recordId;
    uint16_t cowId;
    uint32_t startTime;
    uint32_t endTime;
    float milkWeightKg;
    float milkTemperature;
    uint32_t durationSeconds;
    uint8_t deviceId;
    bool isSynced;
};

/*
==========================================================
 Veterinary Event Record Model
==========================================================
*/
struct VetRecord
{
    uint32_t recordId;
    uint16_t cowId;
    uint8_t eventType;      // e.g., Vaccination, Examination, Treatment
    uint8_t operatorId;
    uint32_t timestamp;
    char notes[64];
    bool isSynced;
};

/*
==========================================================
 System Settings / Configuration Model
==========================================================
*/
struct SystemSettings
{
    uint8_t deviceId;
    DeviceType deviceType;
    uint8_t protocolVersion;
    float calibrationOffset;
    float calibrationFactor;
    char wifiSSID[32];
    char wifiPassword[64];
    uint32_t heartbeatIntervalMs;
};

/*
==========================================================
 Storage Diagnostics & Telemetry Statistics
==========================================================
*/
struct StorageStatistics
{
    uint32_t readOperations;
    uint32_t writeOperations;
    uint32_t failedOperations;
    uint32_t fileCount;
    uint32_t formatCount;
    uint32_t lastErrorTime;
};

#endif
