#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

class RTCDriver
{
public:

    RTCDriver();

    bool begin();

    bool isRunning() const;

    bool adjust(const DateTime& dateTime);

    DateTime now() const;

    String getDateString() const;

    String getTimeString() const;

    String getTimestamp() const;

private:

    RTC_DS1307 rtc;

    bool initialized;

};

#endif
