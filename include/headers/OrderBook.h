#include "Order.h"

using OrderPointer = std::deque<Order>;
using ID_LevelInfos = std::unordered_map<OrderID, std::pair<Order,Time>>;

#ifndef ORDERBOOK_H_INCLUDED
#define ORDERBOOK_H_INCLUDED

class OrderBook
{
private:
    LevelInfos_Ask asks;
    LevelInfos_Buy bids;
    ID_LevelInfos ids;
    std::uint32_t lim = 1000000;
    std::uint32_t PrintSize = 99; // First 50->100
public:
    OrderBook(LevelInfos_Ask &asks, LevelInfos_Buy &bids, ID_LevelInfos ids)
        : asks(asks), bids(bids), ids(ids)
    {}
    //OrderBook(const OrderBook &) = delete;
    //void operator=(const OrderBook &) = delete;
    //OrderBook(OrderBook &&) = delete;
    //void operator=(OrderBook &&) = delete;

public:
    //void SafePopFront(Price price, Time time_, std::map<Price, std::map<Time, LevelPointer>> &Book);
    LevelInfos_Ask GetAsks() { return asks; }
    LevelInfos_Buy GetBids() { return bids; }
    ID_LevelInfos GetIDs() { return ids; }
    void AddBuyOrder(Order &order_, const Time time_);
    void AddSellOrder(Order &order_, const Time time_);
    void TryAskOrder(Order &order, const Time time_, LevelInfo &min_it);
    void TryBuyOrder(Order &order, const Time time_, LevelInfo &min_it);
    Quantity CalcQuantNeeded(std::map<Time, LevelPointer> &pt, Order &order, const Time time_, int i);
    Quantity Calculate(Order &order, const Time time_, std::uint32_t &count, double &avg);
    void BuyLimitOrder(Order &order, const Time time_);
    void SellLimitOrder(Order &order, const Time time_);
    void BuyMarketOrder(Order &order, const Time time_);
    void SellMarketOrder(Order &order, const Time time_);
    void MakeOrder(Order &order, const Time time_);
    void CancelOrder(OrderID orderid, Quantity expected_to_remove);
    void CreateOrder(OrderType type, OrderSide orderside, Time_in_Force tif, unsigned long int &curr_oid_b, unsigned long int &curr_oid_s);
    void ListOwnOrders();
    void Display();
};

#endif // ORDERBOOK_H