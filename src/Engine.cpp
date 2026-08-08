#include "Engine.h"

#include <iostream>

void errorMessage() {
    std::cout << "Error creating order." << std::endl;
}


//Processes orders, deciding if it needs to cancel it or handle it otherwise.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::ProcessOrder(OrderT &order) {

    OrderSide side = order.getOrderSide();

    if (side == OrderSide::Cancel) {
        CancelOrder(order);
        return;
    }

    if (side == OrderSide::Buy || side == OrderSide::Sell) {
        HandleOrder(order);
    }
}

//Sends it toward the execution handler of the book.
//Depending on the result, creates a new order if needed.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::HandleOrder(OrderT &order) {

    ExecutionResultT result = book.handleOrder(order);

    if (
        result.remainingQuantity > 0
        && order.getTimeInForceType() == TimeInForceType::GoodTillCancel
    )
    {
        OrderT remainingOrder = order;

        remainingOrder.setQuantity(result.remainingQuantity);

        remainingOrder.setOrderID(getPreviousBidID() + 1);

        if (order.getOrderSide() == OrderSide::Buy) {
            remainingOrder.setOrderID(nextBidID++);
        }
        else {
            remainingOrder.setOrderID(nextAskID++);
        }

        book.createOrder(remainingOrder);
        AddToUserOrder(remainingOrder);
    }
}


//Creates an order.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::CreateOrder(OrderT &order) {

    if (order.getOrderSide() == OrderSide::Buy) {
        order.setOrderID(nextBidID++);
    }
    else if (order.getOrderSide() == OrderSide::Sell) {
        order.setOrderID(nextAskID++);
    }
    else {
        return;
    }

    book.createOrder(order);
}


//Cancels an order.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::CancelOrder(OrderT &order) {
    book.cancelOrder(order);
}

//Prints the whole book.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::printOrderBook() const {
    book.print();
}


//Prints the orders created by the user.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::printUserOrders() const
{
    std::cout << "\n========== USER ORDERS ==========\n\n";
    for (auto &[orderID, order] : userOrders)
    {
        std::cout << "#" << orderID << " ";

        order.print();
    }
}

//Saves the data of the orders created by the user.
template <
    typename QuantityType,
    typename IDType,
    typename PriceType
>
void Engine<QuantityType, IDType, PriceType>::AddToUserOrders(const OrderT& order) {
    userOrders[order.getOrderID()].push_back(order);
}