#pragma once
#include "Clock.h"
#include <chrono>

const EventTime& Clock::getCurrentTime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t nowTimeT = system_clock::to_time_t(now);
    std::tm *time = std::localtime(&nowTimeT);

    auto msSinceEpoch = duration_cast<milliseconds>(now.time_since_epoch());
    auto ms = msSinceEpoch % 1000;

    std::string hours, minutes, seconds, milliseconds;

    std::string hourAddition, minAddition, secAddition, msAddition;

    hourAddition = time->tm_hour < 10 ? "0" : "";
    hours = std::to_string(time->tm_hour);
    minAddition = time->tm_min < 10 ? "0" : "";
    minutes = std::to_string(time->tm_min);
    secAddition = time->tm_sec < 10 ? "0" : "";
    seconds = std::to_string(time->tm_sec);
    auto msCount = ms.count();
    msAddition = msCount < 100 ? (msCount < 10 ? "00" : "0") : "";
    milliseconds = std::to_string(msCount);

    currentTime.setHour(hours + hourAddition);
    currentTime.setMinutes(minutes + minAddition);
    currentTime.setSeconds(seconds + secAddition);
    currentTime.setMs(milliseconds + msAddition);

    return currentTime;
}