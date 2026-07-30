#include "drivers/storage/sd_driver.h"

SDDriver::SDDriver() : mounted(false) {}

ErrorCode SDDriver::begin()
{
    mounted = SD.begin(SD_CS_PIN);

    if (!mounted)
    {
        mounted = false;
        return ErrorCode::SD_INIT_FAILED;
    }

    ensureDirectory(FilePaths::LOG_DIR);
    ensureDirectory(FilePaths::EVENTS_DIR);
    ensureDirectory(FilePaths::CONFIG_DIR);
    ensureDirectory(FilePaths::EXPORT_DIR);
    ensureDirectory(FilePaths::BACKUP_DIR);

    return ErrorCode::OK;
}

bool SDDriver::isMounted() const { return mounted; }

bool SDDriver::exists(const char* path) const
{
    if (!mounted) return false;
    return SD.exists(path);
}

ErrorCode SDDriver::ensureDirectory(const char* path)
{
    if (!mounted) return ErrorCode::SD_NOT_READY;
    if (SD.exists(path)) return ErrorCode::OK;
    if (SD.mkdir(path)) return ErrorCode::OK;

    return ErrorCode::SD_WRITE_ERROR;
}

ErrorCode SDDriver::createFile(const char* path)
{
    if (!mounted) return ErrorCode::SD_NOT_READY;
    
    // عدم فتح الملف إذا كان موجوداً بالفعل
    if (exists(path)) return ErrorCode::OK;

    File file = SD.open(path, FILE_WRITE);
    if (!file) return ErrorCode::FILE_OPEN_FAILED;
    file.close();
    return ErrorCode::OK;
}

ErrorCode SDDriver::removeFile(const char* path)
{
    if (!mounted) return ErrorCode::SD_NOT_READY;
    if (!SD.exists(path)) return ErrorCode::FILE_NOT_FOUND;
    
    if (!SD.remove(path))
        return ErrorCode::SD_WRITE_ERROR;

    return ErrorCode::OK;
}

ErrorCode SDDriver::append(const char* path, const String& data)
{
    if (!mounted) return ErrorCode::SD_NOT_READY;
    File file = SD.open(path, FILE_APPEND);
    if (!file) return ErrorCode::FILE_OPEN_FAILED;

    // التأكد من نجاح عملية الكتابة
    if (file.print(data) == 0)
    {
        file.close();
        return ErrorCode::SD_WRITE_ERROR;
    }

    file.close();
    return ErrorCode::OK;
}

ErrorCode SDDriver::write(const char* path, const String& data)
{
    if (!mounted) return ErrorCode::SD_NOT_READY;
    File file = SD.open(path, FILE_WRITE);
    if (!file) return ErrorCode::FILE_OPEN_FAILED;

    if (file.print(data) == 0)
    {
        file.close();
        return ErrorCode::SD_WRITE_ERROR;
    }

    file.close();
    return ErrorCode::OK;
}

ErrorCode SDDriver::read(const char* path, String& output)
{
    if (!mounted) return ErrorCode::SD_NOT_READY;
    File file = SD.open(path);
    if (!file) return ErrorCode::FILE_OPEN_FAILED;

    output.clear();
    while (file.available()) {
        output += (char)file.read();
    }
    file.close();
    return ErrorCode::OK;
}

uint64_t SDDriver::cardSizeMB() const
{
    if (!mounted) return 0;
    return SD.cardSize() / (1024ULL * 1024ULL);
}

uint64_t SDDriver::totalSpaceMB() const
{
    if (!mounted) return 0;
    return SD.totalBytes() / (1024ULL * 1024ULL);
}

uint64_t SDDriver::usedSpaceMB() const
{
    if (!mounted) return 0;
    return SD.usedBytes() / (1024ULL * 1024ULL);
}
