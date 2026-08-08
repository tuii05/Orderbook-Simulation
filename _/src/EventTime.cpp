#include "EventTime.h"

//Handles the conversion from a real number to a correctly formatted string.
void EventTime::convertValueToTime(double secondsValue) {
    int hours_ = static_cast<int>(secondsValue / 3600);
    int mins_ = static_cast<int>((static_cast<int>(secondsValue) % 3600) / 60);
    int sec_ = static_cast<int>(secondsValue) % 60;
    int ms_ = static_cast<int>(std::round((secondsValue - std::floor(secondsValue)) * 1000));
    if (ms_ == 1000) {
        ms_ = 0;
        ++sec_;
        if (sec_ == 60)
        {
            sec_ = 0;
            ++mins_;
            if (mins_ == 60)
            {
                mins_ = 0;
                ++hours_;
                hours_ %= 24;
            }
        }
    }

    hours = hours_ < 10 ? "0" + std::to_string(hours_) : std::to_string(hours_);
    minutes = mins_ < 10 ? "0" + std::to_string(mins_) : std::to_string(mins_);
    seconds = sec_ < 10 ? "0" + std::to_string(sec_) : std::to_string(sec_);
    ms = ms_ < 10 ? "0" + std::to_string(ms_) : std::to_string(ms_);
}

// Converts numbers into correctly formatted strings.
void EventTime::setTimeValue(double secondsValue) {
    convertedTimeValue = secondsValue;
    convertValueToTime(secondsValue);
}

// Formats the time for console output.
std::string EventTime::convertToString() const {
    std::ostringstream oss;
    oss << hours << ":" << minutes << ":" << seconds << "." << ms;
    return oss.str();
}
