#include "OrderBook.h"

#include <iostream>
#include <iomanip>

// Creates an order in the book.

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void OrderBook<QuantityType, IDType, PriceType>::createOrder(const OrderT &order) {
    if (order.getOrderSide() == OrderSide::Buy) {
        bidData[order.getPrice()].push_back(order);
    }
    else if (order.getOrderSide() == OrderSide::Sell) {
        askData[order.getPrice()].push_back(order);
    }
}

// Cancels an order in the book.

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void OrderBook<QuantityType, IDType, PriceType>::cancelOrder(const OrderT &order) {
    auto &book = (order.getOrderSide() == OrderSide::Buy) ? bidData : askData;

    auto level = book.find(order.getPrice());

    if (level == book.end()) {
        return;
    }

    auto &orders = level->second;

    auto it = std::find_if(
        orders.begin(),
        orders.end(),
        [&](const OrderT &current) {
                return current.getOrderID() == order.getOrderID();
            }
    );

    if (it != orders.end()) {
        orders.erase(it);
    }

    if (orders.empty()) {
        book.erase(level);
    }
}

// Handles an order in the book.

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
typename OrderBook<QuantityType, IDType, PriceType>::ExecutionResultT
OrderBook<QuantityType, IDType, PriceType>::handleOrder(const OrderT &order)
{
    ExecutionT execution{};
    execution.price = order.getPrice();
    execution.quantity = order.getQuantity();
    execution.orderID = order.getOrderID();
    execution.orderSide = order.getOrderSide();
    execution.orderType = order.getOrderType();
    execution.timeInForce = order.getTimeInForceType();
    execution.time = order.getTimeValue();

    ExecutionResultT result{};

    if (order.getOrderSide() == OrderSide::Buy) {
        result =
            handler.handleExecution(
                execution,
                askData);
    }
    else if (order.getOrderSide() == OrderSide::Sell) {
        result =
            handler.handleExecution(
                execution,
                bidData);
    }
    else {
        return result;
    }

    result = handler.handleTIF(result, execution);

    return result;
}

// Prints the whole book.

template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void OrderBook<QuantityType, IDType, PriceType>::print() const {
    constexpr int maxPrintSize = 50;

    auto printOrder = [](const auto &price, const auto &order) {
        std::cout << "$"
            << std::fixed
            << std::setprecision(2)
            << price
            << " | Quantity: "
            << order.getQuantity()
            << " | Time: "
            << order.getEventTime().convertToString()
            << '\n';
    };

    std::cout << "\n========== ORDER BOOK ==========\n\n";

    std::cout << "\033[1;32m"
        << "BIDS\n"
        << "\033[0m";

    int bidCounter = 0;

    for (const auto &[price, orders] : bidData) {
        for (const auto &order : orders) {
            printOrder(price, order);
            ++bidCounter;
            if (bidCounter >= maxPrintSize) {
                break;
            }
        }
        if (bidCounter >= maxPrintSize) {
            break;
        }
    }

    std::cout << "\n-------------------------------\n";

    std::cout << "\033[1;31m"
        << "ASKS\n"
        << "\033[0m";

    int askCounter = 0;

    for (const auto &[price, orders] : askData) {
        for (const auto &order : orders) {
            printOrder(price, order);
            ++askCounter;
            if (askCounter >= maxPrintSize) {
                break;
            }
        }

        if (askCounter >= maxPrintSize) {
            break;
        }
    }

    std::cout
        << "\n================================\n";
}

//ListOwnOrders' logics needed.