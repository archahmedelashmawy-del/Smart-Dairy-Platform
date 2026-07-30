#pragma once

#include <Arduino.h>

enum class LogLevel : uint8_t
{
    TRACE = 0,

    DEBUG,

    INFO,

    WARNING,

    ERROR,

    CRITICAL
};

class Logger
{
public:

    static void begin();

    static void log(LogLevel level,
                    const String& module,
                    const String& message);

    static void trace(const String& module,
                      const String& message);

    static void debug(const String& module,
                      const String& message);

    static void info(const String& module,
                     const String& message);

    static void warning(const String& module,
                        const String& message);

    static void error(const String& module,
                      const String& message);

    static void critical(const String& module,
                         const String& message);
};
