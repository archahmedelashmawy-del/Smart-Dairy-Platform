#include "drivers/time/rtc_driver.h"

RTCDriver::RTCDriver()
    : initialized(false)
{
}

bool RTCDriver::begin()
{
    initialized = rtc.begin();

    if (!initialized)
        return false;

    if (!rtc.isrunning())
    {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    return true;
}

bool RTCDriver::isRunning() const
{
    if (!initialized)
        return false;

    return rtc.isrunning();
}

bool RTCDriver::adjust(const DateTime& dateTime)
{
    if (!initialized)
        return false;

    rtc.adjust(dateTime);

    return true;
}

DateTime RTCDriver::now() const
{
    if (!initialized)
    {
        return DateTime(2000,1,1,0,0,0);
    }

    return rtc.now();
}

String RTCDriver::getDateString() const
{
    DateTime dt = now();

    char buffer[11];

    snprintf(buffer,
             sizeof(buffer),
             "%04d-%02d-%02d",
             dt.year(),
             dt.month(),
             dt.day());

    return String(buffer);
}

String RTCDriver::getTimeString() const
{
    DateTime dt = now();

    char buffer[9];

    snprintf(buffer,
             sizeof(buffer),
             "%02d:%02d:%02d",
             dt.hour(),
             dt.minute(),
             dt.second());

    return String(buffer);
}

String RTCDriver::getTimestamp() const
{
    DateTime dt = now();

    char buffer[20];

    snprintf(buffer,
             sizeof(buffer),
             "%04d-%02d-%02d %02d:%02d:%02d",
             dt.year(),
             dt.month(),
             dt.day(),
             dt.hour(),
             dt.minute(),
             dt.second());

    return String(buffer);
}
