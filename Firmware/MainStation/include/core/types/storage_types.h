#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

#include <Arduino.h>

/*
==========================================================
 Storage Backend Types
 Supported underlying storage media/drivers
==========================================================
*/
enum class StorageBackend : uint8_t
{
    None = 0,
    SDCard,
    LittleFS,
    SPIFFS,
    NVS
};

/*
==========================================================
 File Access Modes
==========================================================
*/
enum class FileMode : uint8_t
{
    Read = 0,
    Write,      // Overwrites existing
    Append      // Appends to end
};

/*
==========================================================
 Storage Partition/Medium Diagnostics Info
==========================================================
*/
struct StorageInfo
{
    StorageBackend backend;
    bool isMounted;
    uint64_t totalBytes;
    uint64_t usedBytes;
    uint64_t freeBytes;
};

#endif
