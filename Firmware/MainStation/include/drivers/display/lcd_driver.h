#pragma once

#include <Arduino.h>

enum class ScreenID : uint8_t
{
    BOOT,

    HOME,

    IDENTIFICATION,

    PREPARE,

    MILKING,

    SUMMARY,

    ERROR_SCREEN,

    MAINTENANCE
};

class LCDDriver
{
public:

    bool begin();

    void update();

    void clear();

    void setScreen(ScreenID screen);

    ScreenID currentScreen() const;

    void setStatus(const String& text);

    void setMessage(const String& text);

    void refresh();

    bool isReady() const;

private:

    ScreenID activeScreen = ScreenID::BOOT;

    bool initialized = false;
};
