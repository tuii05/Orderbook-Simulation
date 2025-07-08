#include "Definitions.h"

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

inline Time FormatToTime(double seconds)
{
    int hours = static_cast<int>(seconds / 3600);
    int mins = static_cast<int>((static_cast<int>(seconds) % 3600) / 60);
    int sec = static_cast<int>(seconds) % 60;
    int ms = static_cast<int>(std::round((seconds - std::floor(seconds)) * 1000));
    if (ms == 1000)
    {
        ms = 0;
        ++sec;
        if (sec == 60)
        {
            sec = 0;
            ++mins;
            if (mins == 70)
            {
                mins = 0;
                ++hours;
            }
        }
    }
    Time conv_time;
    hours < 10 ? conv_time.hours = "0" + std::to_string(hours) : conv_time.hours = std::to_string(hours);
    mins < 10 ? conv_time.minutes = "0" + std::to_string(mins) : conv_time.minutes = std::to_string(mins);
    sec < 10 ? conv_time.seconds = "0" + std::to_string(sec) : conv_time.seconds = std::to_string(sec);
    ms < 10 ? conv_time.ms = "0" + std::to_string(ms) : conv_time.ms = std::to_string(ms);
    conv_time.value = seconds;
    return conv_time;
}

inline std::string PrintTime(Time time_)
{
    std::ostringstream oss;
    oss << time_.hours << ":" << time_.minutes << ":" << time_.seconds << "." << time_.ms;
    return oss.str();
}

inline Time getTime()
{
    // Get now and milliseconds since epoch
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms_since_epoch = duration_cast<milliseconds>(now.time_since_epoch());
    auto ms_part = ms_since_epoch % 1000;

    // Convert to time_t for localtime
    std::time_t now_time_t = system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&now_time_t);
    Time t;
    // Fill the struct
    t.hours = (tm->tm_hour < 10 ? "0" : "") + std::to_string(tm->tm_hour);
    t.minutes = (tm->tm_min < 10 ? "0" : "") + std::to_string(tm->tm_min);
    t.seconds = (tm->tm_sec < 10 ? "0" : "") + std::to_string(tm->tm_sec);
    t.ms = (ms_part.count() < 100 ? (ms_part.count() < 10 ? "00" : "0") : "") + std::to_string(ms_part.count());

    t.value = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec + ms_part.count() / 1000.0;
    return t;
}

inline void PrintBox()
{
    std::cout << "\n=======================";
    std::cout << "\nPrintout OrderBook [1]";
    std::cout << "\nSubmit Order [2]";
    std::cout << "\n=======================";
    std::cout << "\nYour choice: ";
}

inline void PrintOrderTypes()
{ //-----------------------
    clearScreen();
    std::cout << "\n-----------------------";
    std::cout << "\nOrder Types:";
    std::cout << "\nMarket Order [1]";
    std::cout << "\nLimit Order [2]";
    std::cout << "\nCancel Order [3]";
    std::cout << "\n-----------------------";
    std::cout << "\nChoice: ";
}

inline void PrintTIF()
{
    std::cout << "\n-----------------------";
    std::cout << "\nTime in Force:";
    std::cout << "\nGood Till Cancel [1]";
    std::cout << "\nFill And Kill [2]";
    std::cout << "\n-----------------------";
    std::cout << "\nChoice: ";
}

inline void PrintSide()
{
    std::cout << "\n-----------------------";
    std::cout << "\nOrder Side:";
    std::cout << "\nBuy [1]";
    std::cout << "\nSell [2]";
    std::cout << "\n-----------------------";
    std::cout << "\nChoice: ";
}


#endif // FUNCTIONS_H