#pragma once

#include "EventTime.h"

class Clock
{

private:
    EventTime currentTime;

public:
    Clock() = default;

    const EventTime &getCurrentTime() {
        return currentTime;
    }

    void setTime(double seconds) {
        currentTime.setTimeValue(seconds);
    }
};