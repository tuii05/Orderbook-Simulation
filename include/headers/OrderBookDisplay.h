#pragma once

// Displaying and formatting the orderbook

#include "Engine.h"
#include "Clock.h"

#include <unordered_map>

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
class OrderBookDisplay
{

    using OrderT = Order<QuantityType, IDType, PriceType>;

    using EngineT = Engine<QuantityType, IDType, PriceType>;

private:
    EngineT &engine;
    std::unordered_map<IDType,OrderT> ownOrders;
    Clock clock;

public:
    OrderBookDisplay(EngineT &engine_)
    : engine(engine_)
    {}

    OrderBookDisplay(const OrderBookDisplay &) = delete;
    OrderBookDisplay &operator=(const OrderBookDisplay &) = delete;

    OrderBookDisplay(OrderBookDisplay &&) = delete;
    OrderBookDisplay &operator=(OrderBookDisplay &&) = delete;

public:
    void ListUserOrders() const;

    void DisplayOrders() const;
};