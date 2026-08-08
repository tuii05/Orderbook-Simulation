#pragma once
#include <cstdint>

#define Red "\033[1;31m"
#define Green "\033[1;32m"
#define NC "\033[0m"

enum class OrderSide {
    Buy,
    Sell,
    None
};

enum class OrderType {
    Market,
    Limit,
    Cancel,
    None
};

enum class TimeInForceType {
    GoodTillCancel,
    FillAndKill,
    None
};

using PreferredType = std::uint32_t;
