#pragma once

#include "Order.h"
#include "DataTypes.h"
#include "ExecutionHandler.h"

#include <optional>
#include <utility>

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
class OrderBook {

    using T = DataTypes<QuantityType, IDType, PriceType>;
    using AskLevelData = typename T::AskLevelData;
    using BidLevelData = typename T::BidLevelData;
    using OrderT = typename T::OrderT;
    using ExecutionResultT = typename T::ExecutionResultT;

private:
    AskLevelData askData;

    BidLevelData bidData;

    ExecutionHandler<QuantityType, IDType, PriceType> handler;

public:
    OrderBook() = default;

    OrderBook(AskLevelData ask, BidLevelData bid)
    : askData(std::move(ask))
    , bidData(std::move(bid))
    {}

    OrderBook(const OrderBook &) = delete;
    OrderBook &operator=(const OrderBook &) = delete;

    OrderBook(OrderBook &&) = delete;
    OrderBook &operator=(OrderBook &&) = delete;

public:
    const AskLevelData &getAskData() const {
        return askData;
    }

    const BidLevelData &getBidData() const {
        return bidData;
    }

    std::optional<PriceType> getBestAskPrice() const {
        if (askData.empty()) {
            return std::nullopt;
        }

        return askData.begin()->first;
    }

public:
    ExecutionResultT handleOrder(const OrderT &order);

    void createOrder(const OrderT &order);

    void cancelOrder(const OrderT &order);

    void print() const;
};