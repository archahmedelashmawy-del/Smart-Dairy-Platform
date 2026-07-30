#pragma once

#include "system_state.h"

class StateMachine
{
public:

    void begin(SystemState initialState);

    void update();

    void changeState(SystemState newState);

    SystemState currentState() const;

    SystemState previousState() const;

private:

    SystemState current = SystemState::BOOT;

    SystemState previous = SystemState::BOOT;
};
