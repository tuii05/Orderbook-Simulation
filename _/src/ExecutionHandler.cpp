#include "ExecutionHandler.h"


// No seperate handling logic is needed for market orders.
// However, a decision function can be used to determine which logic
// should be applied to each order type. In this case, a function
// similar to the following is required.

template <typename QuantityType, typename IDType, typename PriceType>
bool ExecutionHandler<QuantityType, IDType, PriceType>::MarketHandleLogic(
    const PriceType &,
    const ExecutionT &
) const {
    return true;
}

// Checks if the price is not too high (for buy) or not too low (for sell)
// in case of limit orders.
template <typename QuantityType, typename IDType, typename PriceType>
bool ExecutionHandler<QuantityType, IDType, PriceType>::LimitHandleLogic(
    const PriceType &levelPrice,
    const ExecutionT &execution
) const {
    const PriceType desiredPrice = execution.price;

    const bool isBuy = execution.orderSide == OrderSide::Buy;

    return isBuy
        ? desiredPrice >= levelPrice
        : desiredPrice <= levelPrice;
}

//Handles the logic for each order type, making the code more readable.
template <typename QuantityType, typename IDType, typename PriceType>
bool ExecutionHandler<QuantityType, IDType, PriceType>::checkLogic(
    const PriceType &levelPrice,
    const ExecutionT &execution
) const {

    const std::size_t value = static_cast<std::size_t>(execution.orderType);

    if (value >= handleLogicFunction.size()) {
        return false;
    }

    return (this->*handleLogicFunction[value])(levelPrice, execution);
}

//Handles the execution of a certain order.
template <typename QuantityType, typename IDType, typename PriceType>
template <typename BookDataType>
typename ExecutionHandler<QuantityType, IDType, PriceType>::ExecutionResultT
ExecutionHandler<QuantityType, IDType, PriceType>::handleExecution(
    ExecutionT &execution,
    BookDataType &data
) {

    using T = DataTypes<QuantityType, IDType, PriceType>;

    static_assert(
        std::is_same_v<BookDataType, typename T::AskLevelData>
        || std::is_same_v<BookDataType, typename T::BidLevelData>
    );

    Quantity remainingQuantity = execution.quantity;

    std::size_t clearedLevels = 0;

    PriceType totalValue{};
    PriceType lastPrice{};

    auto currentLevel = data.begin();

    while (
        currentLevel != data.end() &&
        remainingQuantity > 0
    ) {
        PriceType currentPrice = currentLevel->first;

        if (!checkLogic(currentPrice, execution)) {
            break;
        }

        auto &orders = currentLevel->second;

        while (
            !orders.empty() &&
            remainingQuantity > 0
        ) {
            OrderT &bestOrder = orders.front();

            Quantity tradedQuantity = std::min(
                remainingQuantity,
                bestOrder.getQuantity()
            );

            remainingQuantity -= tradedQuantity;
            totalValue += currentPrice * tradedQuantity;
            lastPrice = currentPrice;

            if (bestOrder.getQuantity() == tradedQuantity) {
                orders.pop();
            }
            else {
                bestOrder.setQuantity(bestOrder.getQuantity() - tradedQuantity);
            }
        }

        if (orders.empty()) {
            currentLevel = data.erase(currentLevel);
            ++clearedLevels;
        }
        else {
            ++currentLevel;
        }
    }

    ExecutionResultT result{};
    result.remainingQuantity = remainingQuantity;
    result.matchedLevels = clearedLevels;
    result.totalValue = totalValue;
    result.lastPrice = lastPrice;
    result.orderID = execution.orderID;

    return result;
}


//Handles the Good-Till-Cancelled time-in-force type, meaning the order
//reamins active until it is fully filled or manually cancelled.
template <typename QuantityType, typename IDType, typename PriceType>
typename ExecutionHandler<QuantityType, IDType, PriceType>::ExecutionResultT
ExecutionHandler<QuantityType, IDType, PriceType>::handleGTC(
    const ExecutionResultT &result,
    const ExecutionT &execution
) {

    ExecutionResultT output = result;

    if (result.remainingQuantity > 0) {
        ExecutionT remainingOrder = execution;

        remainingOrder.quantity = result.remainingQuantity;
        ++remainingOrder.orderID;

        output = remainingOrder;
    }

    return output;
}

// Handles the Fill-and-Kill time-in-force type, meaning the order will
// be cancelled if it cannot be filled immediately.
template <typename QuantityType, typename IDType, typename PriceType>
typename ExecutionHandler<QuantityType, IDType, PriceType>::ExecutionResultT
ExecutionHandler<QuantityType, IDType, PriceType>::handleFK(
    const ExecutionResultT &result,
    const ExecutionT &
) {
    return result;
}


// Handles time-in-force types, deciding which function to call.
template <typename QuantityType, typename IDType, typename PriceType>
typename ExecutionHandler<QuantityType, IDType, PriceType>::ExecutionResultT
ExecutionHandler<QuantityType, IDType, PriceType>::handleTIF(
    const ExecutionResultT &result,
    const ExecutionT &execution
) {

    const std::size_t value = static_cast<std::size_t>(execution.timeInForce);

    if (value >= handleTifFunction.size()) {
        return result;
    }

    return (this->*handleTifFunction[value])(result, execution);
}

// Handles order creations.
template <typename QuantityType, typename IDType, typename PriceType>
template <typename BookDataType>
void ExecutionHandler<QuantityType, IDType, PriceType>::handleCreate(
    const OrderT &order,
    BookDataType &data
) {

    const PriceType price =
        order.getPrice();

    data[price].push(order);
}