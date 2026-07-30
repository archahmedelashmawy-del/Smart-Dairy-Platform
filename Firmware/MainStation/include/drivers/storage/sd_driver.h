#ifndef SD_DRIVER_H
#define SD_DRIVER_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "config/pins.h"
#include "config/file_paths.h"
#include "core/error_codes.h"

class SDDriver
{
public:
    SDDriver();

    ErrorCode begin();
    bool isMounted() const;
    bool exists(const char* path) const;

    ErrorCode createFile(const char* path);
    ErrorCode removeFile(const char* path);
    ErrorCode append(const char* path, const String& data);
    ErrorCode write(const char* path, const String& data);
    ErrorCode read(const char* path, String& output);

    // الإضافات الجديدة
    ErrorCode ensureDirectory(const char* path);
    uint64_t cardSizeMB() const;
    uint64_t totalSpaceMB() const;
    uint64_t usedSpaceMB() const;

private:
    bool mounted;
};

#endif
