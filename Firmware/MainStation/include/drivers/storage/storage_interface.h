#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H

#include <stddef.h>
#include <stdint.h>
#include "core/types/error_codes.h"
#include "core/types/storage_types.h"

class StorageInterface
{
public:

    virtual ~StorageInterface() = default;

    virtual ErrorCode begin() = 0;
    
    virtual void end() = 0;
    
    [[nodiscard]] virtual bool isMounted() const = 0;

    virtual ErrorCode writeFile(
        const char* path, 
        const uint8_t* data, 
        size_t length, 
        FileMode mode = FileMode::Write) = 0;

    virtual ErrorCode loadFile(
        const char* path, 
        uint8_t* buffer, 
        size_t bufferSize, 
        size_t& readLength) = 0;

    virtual ErrorCode deleteFile(const char* path) = 0;

    [[nodiscard]] virtual bool exists(const char* path) const = 0;

    virtual ErrorCode fileSize(const char* path, size_t& size) const = 0;

    virtual ErrorCode createDirectory(const char* path) = 0;

    virtual ErrorCode removeDirectory(const char* path) = 0;

    virtual ErrorCode getInfo(StorageInfo& info) const = 0;

    virtual ErrorCode format() = 0;
};

#endif // STORAGE_INTERFACE_H
