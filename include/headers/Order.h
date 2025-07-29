#include "Functions.h"

#ifndef ORDER_H_INCLUDED
#define ORDER_H_INCLUDED

class Order
{
private:
    OrderType ordertype_;
    OrderID orderid_;
    OrderSide orderside_;
    Price price_;
    Quantity quantity_;
    Time_in_Force tif_;

public:
    Order(OrderType ordertype_, OrderID orderid_, OrderSide orderside_, Price price_, Quantity quantity_, Time_in_Force tif_)
        : ordertype_(ordertype_)
        , orderid_(orderid_)
        , orderside_(orderside_)
        , price_(price_)
        , quantity_(quantity_)
        , tif_(tif_)
    {}
    Order(const Order&) = default;
    Order &operator=(const Order &) = default;
public:
    //Getters
    OrderType getOrderType() { return ordertype_; }
    OrderID getOrderID() { return orderid_; }
    OrderSide getOrderSide() { return orderside_; }
    Price getPrice() { return price_; }
    Quantity getQuantity() { return quantity_; }
    Time_in_Force getTIF() { return tif_; }
    //Setters
    void setQuantity(Quantity val) { quantity_ = val; }
    void setPrice(Price val) { price_ = val; }
    void setOrderSide(OrderSide val) { orderside_ = val; }
    void setOrderID(OrderID val) { orderid_ = val; }
    void setOrderType(OrderType val) { ordertype_ = val; }
};

#endif // ORDER_H