#pragma once

#include "OrderBookDisplay.h"
#include <iostream>

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void OrderBookDisplay<QuantityType, IDType, PriceType>::ListUserOrders() const
{
    engine.printUserOrders();
}

template<
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void OrderBookDisplay<QuantityType,IDType,PriceType>::DisplayOrders() const
{
    engine.printOrderBook();
}