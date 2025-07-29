#include <algorithm>
#include "Orderbook.h"

// Could be helpful: storing iterators so don't have to do asks.find(price_) everytime.

void FindSide(OrderBook &orderbook, std::uint16_t &choice, OrderType type, unsigned long int &curr_oid_b, unsigned long int &curr_oid_s);

int main(void)
{
    // clearScreen();
    // MyTime convtime = FormatToTime(3700.9);
    unsigned long int curr_oid_b = 1;
    unsigned long int curr_oid_s = 1;
    OrderType ordertype_ = OrderType::Limit;
    Time_in_Force tif_ = Time_in_Force::GoodTillCancel;
    Time_in_Force tif_2 = Time_in_Force::FillAndKill;
    OrderSide orderside_ = OrderSide::Sell;
    OrderSide orderside_2 = OrderSide::Buy;
    Order order1 = {ordertype_, 1, orderside_, 100, 10, tif_};
    Order order2 = {ordertype_, 2, orderside_, 50, 10, tif_};
    Order order3 = {ordertype_, 3, orderside_, 150, 10, tif_};
    Order order4 = {ordertype_, 4, orderside_, 200, 10, tif_};
    Order order5 = {ordertype_, 5, orderside_, 100, 5, tif_};
    LevelInfos_Ask sell;
    LevelInfos_Buy buy;
    ID_LevelInfos ids;
    OrderBook orderbook(sell, buy, ids);
    Time time_ = FormatToTime(1);
    orderbook.AddSellOrder(order1, time_); // 100 10
    orderbook.AddBuyOrder(order1, time_);
    ++curr_oid_s;
    ++curr_oid_b;
    time_.value += 1;
    orderbook.AddSellOrder(order2, time_); // 50 10
    orderbook.AddBuyOrder(order2, time_);
    ++curr_oid_s;
    ++curr_oid_b;
    time_.value += 1;
    orderbook.AddSellOrder(order3, time_); //
    orderbook.AddBuyOrder(order3, time_);
    ++curr_oid_s;
    ++curr_oid_b;
    time_.value += 1;
    orderbook.AddSellOrder(order4, time_);
    orderbook.AddBuyOrder(order4, time_);
    ++curr_oid_s;
    ++curr_oid_b;
    time_.value = 0;
    orderbook.AddSellOrder(order5, time_);
    orderbook.AddBuyOrder(order5, time_);
    ++curr_oid_s;
    ++curr_oid_b;
    orderbook.Display();
    Order order6 = {ordertype_, 6, orderside_, 100, 20, tif_};
    orderbook.SellMarketOrder(order6, time_);
    ++curr_oid_s;
    // Order order6 = {ordertype_, 1, orderside_, 200, 30, tif_};
    // orderbook.BuyLimitOrder(order6, time_);
    // Order order8 = {ordertype_, 8, orderside_, 100, 5, tif_};
    // orderbook.SellLimitOrder(order8, time_);
    orderbook.Display();
    orderbook.ListOwnOrders();
    // time_.value += 1;
    // orderbook.BuyOrder(order5, time_);
    // orderbook.Display();
    // orderbook.CancelOrder(5, 20);
    Price price_ = 100;
    Quantity quantity_ = 20;
    orderbook.ListOwnOrders();
    for (int i = 0; i < 2000000; ++i)
    {
        if (i % 9 == 0)
        {
            order6.setPrice(++price_);
            order6.setQuantity(++quantity_);
        }
        else
        {
            order6.setPrice(--price_);
            order6.setQuantity(++quantity_);
        }
        if (price_ <= 0)
            price_ = 100;
        if (quantity_ <= 0)
            quantity_ = 20;
        if (i % 2 == 0)
        {
            orderside_ = OrderSide::Buy;
            ++curr_oid_b;
            orderbook.AddBuyOrder(order6, time_);
        }
        else
        {
            orderside_ = OrderSide::Sell;
            ++curr_oid_s;
            orderbook.AddSellOrder(order6, time_);
        }
        ++time_.value;
    }
    std::uint16_t choice;
    OrderType type;
    OrderSide orderside;
    clearScreen();
    while (true)
    {
        orderbook.Display();
        std::cout << "\n";
        PrintBox();
        std::cin >> choice;
        switch (choice)
        {
        case 1:
            // Show Display Again
            clearScreen();
            break;
        case 2:
            // Submit order
            PrintOrderTypes();
            std::cin >> choice;
            switch (choice)
            {
            case 1:
                // Market
                type = OrderType::Market;
                FindSide(orderbook, choice, type, curr_oid_b, curr_oid_s);
                break;
            case 2:
                // Limit
                type = OrderType::Limit;
                FindSide(orderbook, choice, type, curr_oid_b, curr_oid_s);
                break;
            case 3:
                // Cancel
                type = OrderType::Cancel;
                orderbook.CreateOrder(type, orderside, Time_in_Force::None, curr_oid_b, curr_oid_s);
                break;
            default:
                std::cout << "There's no choice like that.";
                break;
            }
            break;
        default:
            std::cout << "There's no choice like that.";
            break;
        }
    }
    system("Pause");
    return 1;
}

void FindSide(OrderBook &orderbook, std::uint16_t &choice, OrderType type, unsigned long int &curr_oid_b, unsigned long int &curr_oid_s)
{
    OrderSide orderside;
    Time_in_Force tif_n;

    PrintSide();
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        orderside = OrderSide::Buy;
        break;
    case 2:
        orderside = OrderSide::Sell;
        break;
    default:
        std::cout << "There's no choice like that.\n";
        return;
    }

    PrintTIF();
    std::cin >> choice;

    switch (choice)
    {
    case 1:
        tif_n = Time_in_Force::GoodTillCancel;
        break;
    case 2:
        tif_n = Time_in_Force::FillAndKill;
        break;
    default:
        std::cout << "There's no choice like that.\n";
        return;
    }
    orderbook.CreateOrder(type, orderside, tif_n, curr_oid_b, curr_oid_s);
}