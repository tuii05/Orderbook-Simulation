#pragma once
#include "ExecutionResult.h"
#include "Order.h"
#include "Execution.h"

#include <queue>
#include <map>
#include <functional>

template<typename QuantityType, typename IDType, typename PriceType>
struct DataTypes {
    using OrderData = std::queue<Order<QuantityType, IDType, PriceType>>;
    using AskLevelData = std::map<PriceType, OrderData>;
    using BidLevelData = std::map<PriceType, OrderData, std::greater<PriceType>>;
    using OrderT = Order<QuantityType, IDType, PriceType>;
    using ExecutionT = Execution<QuantityType, IDType, PriceType>;
    using Quantity = QuantityType;
    using Price = PriceType;
    using ExecutionResultT = ExecutionResult<QuantityType, IDType, PriceType>;
};