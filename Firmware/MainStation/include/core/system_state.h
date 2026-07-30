#pragma once

#include <stdint.h>

enum class SystemState : uint8_t
{
    BOOT = 0,

    INITIALIZATION,

    IDLE,

    WAIT_FOR_IDENTIFICATION,

    VALIDATE_COW,

    PREPARE_MILKING,

    PUMP_DELAY,

    MILKING,

    STOPPING,

    SAVE_SESSION,

    LOCAL_SYNC,

    REMOTE_SYNC,

    ERROR_STATE,

    MAINTENANCE
};
