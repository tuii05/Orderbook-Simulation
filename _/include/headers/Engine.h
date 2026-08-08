#pragma once

#include "Execution.h"
#include "OrderBook.h"

#include <array>
#include <unordered_map>

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
class Engine
{

    using T = DataTypes<QuantityType, IDType, PriceType>;
    using OrderT = typename T::OrderT;
    using OrderBookT = OrderBook<QuantityType, IDType, PriceType>;
    using ExecutionT = typename T::ExecutionT;

private:
    OrderBookT book;
    std::unordered_map<IDType, OrderT> userOrders;

    IDType nextAskID;
    IDType nextBidID;

public:
    Engine(OrderBookT book_)
    : book(std::move(book_))
    , nextAskID(1)
    , nextBidID(1)
    {}

    Engine()
    : nextAskID(1)
    , nextBidID(1)
    {}

    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;

    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

public:
    void ProcessOrder(OrderT &order);
    void HandleOrder(OrderT &order);
    void CancelOrder(OrderT &order);
    void CreateOrder(OrderT &order);

public:
    void AddToUserOrders(const OrderT &order);

private:
    void HandleGTC(ExecutionT &execution);
    void HandleFK(ExecutionT &execution);
    using tifHandler = void (Engine::*)(ExecutionT &);

    std::array<tifHandler, 2> handleTifFunction = {
        &Engine::HandleGTC,
        &Engine::HandleFK
    };

public:
    IDType getPreviousAskID() const {
        return nextAskID - 1;
    }

    IDType getPreviousBidID() const {
        return nextBidID - 1;
    }

    void printOrderBook() const;
    void printUserOrders() const;

private:
    void RemainingOrder(OrderT &order);

};
