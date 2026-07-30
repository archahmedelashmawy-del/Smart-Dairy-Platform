#pragma once

class Watchdog
{
public:

    static void begin();

    static void reset();

    static void feed();

    static bool isEnabled();
};
