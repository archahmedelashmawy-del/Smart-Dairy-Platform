#include "drivers/storage/sd_driver.h"

SDDriver::SDDriver()
    : mounted(false)
{
}

ErrorCode SDDriver::begin()
{
    mounted = SD.begin(SD_CS_PIN);

    if (!mounted)
        return ErrorCode::SD_INIT_FAILED;

    return ErrorCode::OK;
}

bool SDDriver::isMounted() const
{
    return mounted;
}

bool SDDriver::exists(const char* path) const
{
    if (!mounted)
        return false;

    return SD.exists(path);
}

ErrorCode SDDriver::createFile(const char* path)
{
    if (!mounted)
        return ErrorCode::SD_NOT_READY;

    File file = SD.open(path, FILE_WRITE);

    if (!file)
        return ErrorCode::FILE_OPEN_FAILED;

    file.close();

    return ErrorCode::OK;
}

ErrorCode SDDriver::removeFile(const char* path)
{
    if (!mounted)
        return ErrorCode::SD_NOT_READY;

    if (!SD.exists(path))
        return ErrorCode::FILE_NOT_FOUND;

    SD.remove(path);

    return ErrorCode::OK;
}

ErrorCode SDDriver::append(const char* path,
                           const String& data)
{
    if (!mounted)
        return ErrorCode::SD_NOT_READY;

    File file = SD.open(path, FILE_APPEND);

    if (!file)
        return ErrorCode::FILE_OPEN_FAILED;

    file.print(data);

    file.close();

    return ErrorCode::OK;
}

ErrorCode SDDriver::write(const char* path,
                          const String& data)
{
    if (!mounted)
        return ErrorCode::SD_NOT_READY;

    File file = SD.open(path, FILE_WRITE);

    if (!file)
        return ErrorCode::FILE_OPEN_FAILED;

    file.print(data);

    file.close();

    return ErrorCode::OK;
}

ErrorCode SDDriver::read(const char* path,
                         String& output)
{
    if (!mounted)
        return ErrorCode::SD_NOT_READY;

    File file = SD.open(path);

    if (!file)
        return ErrorCode::FILE_OPEN_FAILED;

    output.clear();

    while (file.available())
    {
        output += (char)file.read();
    }

    file.close();

    return ErrorCode::OK;
}
