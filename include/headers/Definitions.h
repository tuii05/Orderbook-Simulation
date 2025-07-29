#include <iostream>
#include <cstdint>
#include <list>
#include <map>
#include <deque>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>

#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

inline void clearScreen()
{
    std::cout << "\033[2J\033[1;1H";
}

struct Time
{
    std::string hours;
    std::string minutes;
    std::string seconds;
    std::string ms;
    double value;
    Time() = default;
    Time(const Time &) = default;
    Time &operator=(const Time &) = default;
    bool operator<(const Time &other) const
    {
        return value < other.value;
    }
    bool operator>(const Time &other) const
    {
        return value > other.value;
    }
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderID = std::int64_t;

// OrderBook
struct LevelInfo
{
    OrderID orderid_;
    Price price_;
    Quantity quantity_;
    Time time_;
};

using LevelPointer = std::deque<LevelInfo>;
using Leveliterator = LevelPointer::iterator;

using LevelInfos_Ask = std::map<Price, std::map<Time, LevelPointer>>;
using LevelInfos_Buy = std::map<Price, std::map<Time, LevelPointer>, std::greater<int>>;

// Orders
enum class OrderSide
{
    Buy,
    Sell,
    None,
};

enum class OrderType
{
    Market,
    Limit,
    Cancel,
};

enum class Time_in_Force
{
    GoodTillCancel,
    FillAndKill,
    None,
};

#endif // DEFINITIONS_H