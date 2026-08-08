#pragma once
//Representing an order

#include "Def.h"
#include "EventTime.h"

template<typename QuantityType, typename IDType, typename PriceType>
class Order {
    private:
        PriceType price;
        QuantityType quantity;
        IDType orderID;
        OrderType orderType;
        OrderSide orderSide;
        TimeInForceType timeInForce;
        EventTime eventTime;
    public:
        Order(PriceType price, QuantityType quantity, IDType orderID, OrderType orderType, OrderSide orderSide, TimeInForceType timeInForce, const EventTime& eventTime)
        : price(price)
        , quantity(quantity)
        , orderID(orderID)
        , orderType(orderType)
        , orderSide(orderSide)
        , timeInForce(timeInForce)
        , eventTime(eventTime)
        {};

        Order()
        : price(0)
        , quantity(0)
        , orderID(0)
        , orderType(OrderType::None)
        , orderSide(OrderSide::None)
        , timeInForce(TimeInForceType::None)
        , eventTime()
        {};

        Order(const Order &) = default;
        Order &operator=(const Order &) = default;

        Order(Order &&) = default;
        Order &operator=(Order &&) = default;

    public:
        PriceType getPrice() const { return price; }
        QuantityType getQuantity() const { return quantity; }
        IDType getOrderID() const { return orderID; }
        OrderType getOrderType() const { return orderType; }
        OrderSide getOrderSide() const { return orderSide; }
        TimeInForceType getTimeInForceType() const { return timeInForce; }
        double getTimeValue() const { return eventTime.getTimeValue(); }
        const EventTime& getEventTime() const { return eventTime; }

    public:
        void setPrice(PriceType price_) { price = price_; }
        void setQuantity(QuantityType quantity_) { quantity = quantity_; }
        void setOrderID(IDType orderID_) { orderID = orderID_; }
        void setOrderType(OrderType orderType_) { orderType = orderType_; }
        void setOrderSide(OrderSide orderSide_) { orderSide = orderSide_; }
        void setTimeInForceType(TimeInForceType timeInForce_) { timeInForce = timeInForce_; }
        void setTimeValue(double seconds) { eventTime.setTimeValue(seconds); }

    public:
        void print() const;
};
