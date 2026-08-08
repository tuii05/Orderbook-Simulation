// Represents an execution done by the engine.
#pragma once
#include "Def.h"


template <typename QuantityType, typename IDType, typename PriceType>
struct Execution {
    PriceType price{};
    QuantityType quantity{};
    IDType orderID{};
    OrderSide orderSide{OrderSide::None};
    OrderType orderType{OrderType::None};
    TimeInForceType timeInForce{TimeInForceType::None};
    double time;
};
