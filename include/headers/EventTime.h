#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>

class EventTime {
    private:
        std::string hours;
        std::string minutes;
        std::string seconds;
        std::string ms;
        double convertedTimeValue;

    public:
        EventTime() : convertedTimeValue(0.0) {}
        EventTime(const EventTime &) = default;
        EventTime &operator=(const EventTime &) = default;

    public:
        double getTimeValue() const { return convertedTimeValue; }

    public:
        void convertValueToTime(double secondsValue); //convert a given seconds to {hours,mins,secs,ms} strings
        void setTimeValue(double secondsValue); //set the new value of the event time

    public:
        std::string convertToString() const;

    public:
        bool operator<(const EventTime& other) const {
            return (convertedTimeValue < other.getTimeValue());
        }

        bool operator>(const EventTime& other) const {
            return (convertedTimeValue > other.getTimeValue());
        }

    public:
        void setHour(const std::string hours_) {
            hours = hours_;
        }

        void setMinutes(const std::string minutes_) {
            minutes = minutes_;
        }

        void setSeconds(const std::string seconds_) {
            seconds = seconds_;
        }

        void setMs(const std::string ms_) {
            ms = ms_;
        }
};
