#ifndef STORAGE_MODELS_H
#define STORAGE_MODELS_H

#include <Arduino.h>
#include "core/types/system_types.h"

/*
==========================================================
 Veterinary Event Types
==========================================================
*/
enum class VetEventType : uint16_t
{
    Unknown = 0,
    Vaccination,
    Examination,
    Treatment,
    Insemination,
    PregnancyCheck
};

/*
==========================================================
 Milking Session Record Model
==========================================================
*/
struct MilkingRecord
{
    uint32_t cowID;
    uint32_t startTime;
    uint32_t endTime;
    float milkWeight;
    float milkVolume;
    float temperature;
    uint32_t durationSeconds;
};

/*
==========================================================
 Veterinary Event Record Model
==========================================================
*/
struct VetRecord
{
    uint32_t cowID;
    VetEventType eventType;
    uint32_t operatorID;
    uint32_t timestamp;
};

/*
==========================================================
 System Settings Model
==========================================================
*/
struct SystemSettings
{
    uint16_t deviceID;
    char deviceName[32];
    DeviceType deviceType;
    float calibrationFactor;
    uint32_t heartbeatIntervalMs;
    uint8_t protocolVersion;
};

/*
==========================================================
 Storage Telemetry & Statistics Model
==========================================================
*/
struct StorageStatistics
{
    uint32_t totalWrites;
    uint32_t totalReads;
    uint32_t writeFailures;
    uint32_t readFailures;
    uint32_t corruptionsDetected;
    uint64_t bytesWritten;
    uint64_t bytesRead;
};

#endif // STORAGE_MODELS_H
