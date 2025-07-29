#include "Orderbook.h"
#include <algorithm>
#include <execution>

template<typename BookType>
void SafePopFront(Price price, Time time_, BookType& Book) {
    auto iteratorPrice = Book.find(price);
    if(iteratorPrice != Book.end()) {
        auto &mapNeeded = iteratorPrice->second;
        auto iteratorTime = mapNeeded.find(time_);
        if(iteratorTime != mapNeeded.end() && !iteratorTime->second.empty()) {
            iteratorTime->second.pop_front();
            if(iteratorTime->second.empty()) {
                mapNeeded.erase(iteratorTime);
                if(mapNeeded.empty()) {
                    Book.erase(iteratorPrice);
                }
            }
        }
    }
}


void OrderBook::AddSellOrder(Order &order_, const Time time_)
{
    LevelInfo newlevel = {
        order_.getOrderID(),
        order_.getPrice(),
        order_.getQuantity(),
        time_};
    asks[order_.getPrice()][time_].push_back({newlevel});
    order_.setOrderSide(OrderSide::Sell);
    while (asks.size() > lim)
    {
        asks.erase(std::prev(asks.end()));
    }
}

void OrderBook::AddBuyOrder(Order &order_, const Time time_)
{
    LevelInfo newlevel = {
        order_.getOrderID(),
        order_.getPrice(),
        order_.getQuantity(),
        time_};
    bids[order_.getPrice()][time_].push_back({newlevel});
    order_.setOrderSide(OrderSide::Buy);
    while (bids.size() >= lim)
    {
        bids.erase(std::prev(bids.end()));
    }
}

void OrderBook::TryAskOrder(Order &order, const Time time_, LevelInfo& min_it)
{
    Quantity quantity_desired = order.getQuantity();
    Quantity quantity_has = min_it.quantity_;

    if (quantity_has > quantity_desired)
    {
        min_it.quantity_ -= quantity_desired;
        order.setQuantity(0);
    }
    else
    {
        order.setQuantity(quantity_desired - quantity_has);
        SafePopFront(order.getPrice(), time_, asks);
        //if (!asks[order.getPrice()][time_].empty()) asks[order.getPrice()][time_].pop_front();
    }
}

void OrderBook::TryBuyOrder(Order &order, const Time time_, LevelInfo &min_it)
{
    Quantity quantity_desired = order.getQuantity();
    Quantity quantity_has = min_it.quantity_;

    if (quantity_has > quantity_desired)
    {
        min_it.quantity_ -= quantity_desired;
        order.setQuantity(0);
    }
    else
    {
        order.setQuantity(quantity_desired - quantity_has);
        SafePopFront(order.getPrice(), time_, bids);
        //if (!bids[order.getPrice()][time_].empty()) bids[order.getPrice()][time_].pop_front();
    }
}

Quantity OrderBook::CalcQuantNeeded(std::map<Time,LevelPointer> &pt, Order &order, const Time time_, int i)
{
    //auto start = pt.begin();
    //auto end = pt.end();
    //auto min_it = std::min_element(
    //    std::execution::par_unseq,
    //    start,
    //    end,
    //    [](const LevelInfo &a, const LevelInfo &b)
    //    {
    //        return (a.time_.value < b.time_.value) || (a.time_.value == b.time_.value && a.quantity_ > b.quantity_);
    //    });
    auto &curr_list = pt.begin()->second;
    auto &min_it = curr_list.front();
    Quantity before_ = order.getQuantity();
    i == 1 ? TryAskOrder(order, time_, min_it) : TryBuyOrder(order, time_, min_it);
    return before_ - order.getQuantity();
}

Quantity OrderBook::Calculate(Order &order, const Time time_, std::uint32_t &count, double &avg)
{
    bool isBuy = (order.getOrderSide() == OrderSide::Buy);
    Price price_ = order.getPrice();
    Price price_before = price_;
    avg += price_;
    Quantity quantity_needed = order.getQuantity();
    if (isBuy)
    {
        while (order.getQuantity() > 0)
        {
            auto it = asks.find(order.getPrice());
            if (it == asks.end())
                break;
            auto &pt = it->second;
            if (pt.empty())
            {
                if (it == asks.end())
                    break;
                ++count;
                auto to_erase = it;
                ++it;
                asks.erase(to_erase);
                price_ = it->first;
                order.setPrice(price_);
                continue;
            }
            if (price_before != price_)
            {
                avg += price_;
                price_before = price_;
            }
            Quantity before = order.getQuantity();
            quantity_needed -= CalcQuantNeeded(pt, order, time_, 1);
            if (order.getQuantity() == before)
                break;
        }
    }
    else
    {
        while (order.getQuantity() > 0)
        {
            auto it = bids.find(order.getPrice());
            if (it == bids.end())
                break;
            auto &pt = it->second;
            if (pt.empty())
            {
                if (it == bids.end())
                    break;
                auto to_erase = it;
                ++it;
                ++count;
                bids.erase(to_erase);
                price_ = it->first;
                order.setPrice(price_);
                continue;
            }
            if (price_before != price_)
            {
                avg += price_;
                price_before = price_;
            }
            Quantity before = order.getQuantity();
            quantity_needed -= CalcQuantNeeded(pt, order, time_, 0);
            if (order.getQuantity() == before)
                break;
        }
    }
    return quantity_needed;
}

void OrderBook::BuyLimitOrder(Order &order, const Time time_)
{
    const Price max_price = order.getPrice();
    Quantity quantity_needed = order.getQuantity();
    Quantity original_quantity = quantity_needed;
    std::uint32_t matched_levels = 0;
    double total_cost = 0.0;

    auto it = asks.begin();

    while (it != asks.end() && it->first <= max_price && quantity_needed > 0)
    {
        auto &order_list = it->second;
        Quantity before = quantity_needed;

        while (!order_list.empty() && quantity_needed > 0)
        {
            auto& curr_list = order_list.begin()->second;
            if (curr_list.empty()) {
                asks.erase(it);
                break;
            }
            // auto min_it = (order_list.size() > 1)
            //                   ? std::min_element(
            //                         std::execution::par_unseq,
            //                         order_list.begin(),
            //                         order_list.end(),
            //                        [](const LevelInfo &a, const LevelInfo &b)
            //                         { return (a.time_.value < b.time_.value) || (a.time_.value == b.time_.value && a.quantity_ > b.quantity_); })
            //                   : order_list.begin();
            auto &min_it = curr_list.front();
            Quantity available = min_it.quantity_;
            if (available > quantity_needed)
            {
                min_it.quantity_ -= quantity_needed;
                quantity_needed = 0;
            }
            else
            {
                quantity_needed -= available;
                curr_list.pop_front();
                if (curr_list.empty())
                {
                    order_list.erase(order_list.begin());
                }
            }
        }

        if (quantity_needed < before)
        {
            ++matched_levels;
            total_cost += it->first;
        }

        if (order_list.empty())
        {
            it = asks.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (order.getTIF() == Time_in_Force::GoodTillCancel)
    {
        if (quantity_needed > 0)
        {
            order.setQuantity(quantity_needed);
            AddBuyOrder(order, time_);
            std::cout << "\n[Limit-GTC] Buy order was filled " << (original_quantity - quantity_needed) << "/" << original_quantity << " at an average price of $" << (total_cost / matched_levels) << ".\n";
            std::cout << "\n[Limit-GTC] Buy order placed for " << order.getQuantity() << " items at $" << order.getPrice() << ".\n";
            //ids[order.getOrderID()] = std::make_pair(order,time_);
            ids.insert_or_assign(order.getOrderID(), std::make_pair(order, time_));
        }
        else
        {
            std::cout << "\n[Limit-GTC] Entire order filled";
            if (matched_levels > 0)
                std::cout << " at an average price of $" << (total_cost / matched_levels);
            std::cout << ".\n";
        }
    }
    else if (order.getTIF() == Time_in_Force::FillAndKill)
    {
        if (quantity_needed > 0)
        {
            std::cout << "\n[Limit-FAK] " << quantity_needed << " items could not be bought at or below $" << order.getPrice() << ".\n";
        }
        else
        {
            std::cout << "\n[Limit-FAK] Entire order filled";
            if (matched_levels > 0)
                std::cout << " at average price $" << (total_cost / matched_levels);
            std::cout << ".\n";
        }
    }
}

void OrderBook::SellLimitOrder(Order &order, const Time time_)
{
    const Price min_price = order.getPrice();
    Quantity quantity_needed = order.getQuantity();
    Quantity original_quantity = quantity_needed;
    std::uint32_t matched_levels = 0;
    double total_cost = 0.0;

    auto it = bids.begin();

    while (it != bids.end() && it->first >= min_price && quantity_needed > 0)
    {
        auto &order_list = it->second;
        Quantity before = quantity_needed;

        while (!order_list.empty() && quantity_needed > 0)
        {
            auto &curr_list = order_list.begin()->second;
            if (curr_list.empty())
            {
                break;
            }
            //auto min_it = (order_list.size() > 1)
            //                  ? std::min_element(
            //                        std::execution::par_unseq,
            //                        order_list.begin(),
            //                        order_list.end(),
            //                        [](const LevelInfo &a, const LevelInfo &b)
            //                        { return (a.time_.value < b.time_.value) || (a.time_.value == b.time_.value && a.quantity_ > b.quantity_); })
            //                  : order_list.begin();

            auto &min_it = curr_list.front();

            Quantity available = min_it.quantity_;
            if (available > quantity_needed)
            {
                min_it.quantity_ -= quantity_needed;
                quantity_needed = 0;
            }
            else
            {
                quantity_needed -= available;
                curr_list.pop_front();
                if (curr_list.empty())
                {
                    order_list.erase(order_list.begin());
                }
            }
        }

        if (quantity_needed < before)
        {
            ++matched_levels;
            total_cost += it->first;
        }

        if (order_list.empty())
        {
            it = bids.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (order.getTIF() == Time_in_Force::GoodTillCancel)
    {
        if (quantity_needed > 0)
        {
            order.setQuantity(quantity_needed);
            AddSellOrder(order, time_);
            if (matched_levels > 0)
                std::cout << "\n[Limit-GTC] Sell order was filled " << (original_quantity - quantity_needed) << "/" << original_quantity << " at an average price of $" << (total_cost / matched_levels) << ".\n";
            std::cout << "\n[Limit-GTC] Sell order placed for " << order.getQuantity() << " items at $" << order.getPrice() << ".\n";
            //ids[order.getOrderID()] = std::make_pair(order,time_);
            ids.insert_or_assign(order.getOrderID(), std::make_pair(order, time_));
        }
        else
        {
            std::cout << "\n[Limit-GTC] Entire order filled";
            if (matched_levels > 0)
                std::cout << " at an average price of $" << (total_cost / matched_levels);
            std::cout << ".\n";
        }
    }
    else if (order.getTIF() == Time_in_Force::FillAndKill)
    {
        if (quantity_needed > 0)
        {
            std::cout << "\n[Limit-FAK] " << quantity_needed << " items could not be sold at or below $" << order.getPrice() << ".\n";
        }
        else
        {
            std::cout << "\n[Limit-FAK] Entire order filled";
            if (matched_levels > 0)
                std::cout << " at an average price of $" << (total_cost / matched_levels);
            std::cout << ".\n";
        }
    }
}

void OrderBook::BuyMarketOrder(Order &order, const Time time_)
{
    Price price_ = asks.begin()->first;
    order.setPrice(price_);
    Price original_price = price_;
    std::uint32_t count = 1;
    double avg = 0;
    Quantity quantity_ = order.getQuantity();
    Quantity quantity_needed = quantity_;
    order.setOrderSide(OrderSide::Buy);
    order.setOrderType(OrderType::Market);
    auto it = asks.begin();
    if (it == asks.end())
    {
        std::cout << "\n[Market] Couldn't find any sell order for this item.\n";
    }
    else
    {
        auto &pt = it->second;
        if (pt.size() < 1)
        {
            std::cout << "\n[Market Buy] Unknown error while running.\n";
        }
        else
        {
            while (order.getQuantity() > 0 && !pt.empty())
            {
                quantity_needed -= CalcQuantNeeded(pt, order, time_, 1);
            }
        }
        if (order.getTIF() == Time_in_Force::GoodTillCancel)
        {
            quantity_needed = Calculate(order, time_, count, avg);
            if (quantity_needed > 0)
            {
                order.setPrice(original_price);
                std::cout << "\n[Market] A buy order was created for " << order.getQuantity() << " items at a price of $" << order.getPrice() << ".\n";
                AddBuyOrder(order, time_);
                //ids[order.getOrderID()] = std::make_pair(order,time_);
                ids.insert_or_assign(order.getOrderID(), std::make_pair(order, time_));
            }
            else
            {
                std::cout << "\n[Market] All of the items have been bought at an average price of $" << avg / count << ".\n";
            }
            it = asks.find(order.getPrice());
            if (it != asks.end() && it->second.empty())
                asks.erase(it);
        }
        else if (order.getTIF() == Time_in_Force::FillAndKill)
        {
            quantity_needed = Calculate(order, time_, count, avg);
            if (quantity_needed > 0)
            {
                std::cout << "\n[Market] " << quantity_needed << " items couldn't be bought at a price of $" << order.getPrice() << ".\n";
            }
            else
            {
                std::cout << "\n[Market] All of the items have been bought at an average price of $" << avg / count << ".\n";
            }
            it = asks.find(order.getPrice());
            if (it != asks.end() && it->second.empty())
                asks.erase(it);
        }
    }
}

void OrderBook::SellMarketOrder(Order &order, const Time time_)
{
    Price price_ = bids.begin()->first;
    order.setPrice(price_);
    Price original_price = price_;
    Quantity quantity_ = order.getQuantity();
    Quantity quantity_needed = quantity_;
    order.setOrderSide(OrderSide::Sell);
    order.setOrderType(OrderType::Market);
    std::uint32_t count = 1;
    double avg = 0;
    auto it = bids.begin();
    if (it == bids.end())
    {
        std::cout << "\n[Market] Couldn't find any buy order for this item.";
    }
    else
    {
        auto &pt = it->second;
        if (pt.size() < 1)
        {
            std::cout << "\n[Market] Unknown error while running.";
        }
        else
        {
            while (order.getQuantity() > 0 && !pt.empty())
            {
                quantity_needed -= CalcQuantNeeded(pt, order, time_, 0);
            }
        }
        if (order.getTIF() == Time_in_Force::GoodTillCancel)
        {
            quantity_needed = Calculate(order, time_, count, avg);
            if (quantity_needed > 0)
            {
                order.setPrice(original_price);
                order.setOrderSide(OrderSide::Sell);
                std::cout << "\n[Market] A sell order was created for " << order.getQuantity() << " items at a price of " << order.getPrice() << ".\n";
                AddSellOrder(order, time_);
                //ids[order.getOrderID()] = std::make_pair(order, time_);
                ids.insert_or_assign(order.getOrderID(), std::make_pair(order, time_));
            }
            else
            {
                std::cout << "\n[Market] All of the items have been sold at an average price of $" << avg / count << ".\n";
            }
            it = bids.find(order.getPrice());
            if (it != bids.end() && it->second.empty())
                bids.erase(it);
        }
        else if (order.getTIF() == Time_in_Force::FillAndKill)
        {
            quantity_needed = Calculate(order, time_, count, avg);
            if (quantity_needed > 0)
            {
                std::cout << "\n[Market] " << quantity_needed << " items couldn't be bought.\n";
            }
            else
            {
                std::cout << "\n[Market] All of the items have been sold at an average price of $" << avg / count << ".\n";
            }
            it = bids.find(order.getPrice());
            if (it != bids.end() && it->second.empty())
                bids.erase(it);
        }
    }
}

void OrderBook::MakeOrder(Order &order, const Time time_)
{
    if (order.getOrderType() == OrderType::Limit)
    {
        order.getOrderSide() == OrderSide::Buy ? BuyLimitOrder(order, time_) : SellLimitOrder(order, time_);
    }
    else if (order.getOrderType() == OrderType::Market)
    {
        order.getOrderSide() == OrderSide::Buy ? BuyMarketOrder(order, time_) : SellMarketOrder(order, time_);
    }
}

void OrderBook::CancelOrder(OrderID orderID, Quantity expected_to_remove) {
    auto order_iterator = ids.find(orderID);
    if (order_iterator == ids.end())
    {
        std::cout << std::endl << "Invalid Order ID!" << std::endl;
        return;
    }
    else {
        Price priceNeeded = order_iterator->second.first.getPrice();
        Time timeNeeded = order_iterator->second.second;
        OrderSide side = order_iterator->second.first.getOrderSide();
        if(side == OrderSide::None) {
            std::cout << std::endl <<"Order has _None_ set as side, removing it entirely." << std::endl;
            ids.erase(orderID);
            return;
        }
        LevelPointer& sideNeeded = (side == OrderSide::Buy) ? bids[priceNeeded][timeNeeded] : asks[priceNeeded][timeNeeded];
        auto start = sideNeeded.begin();
        auto end = sideNeeded.end();
        if(!sideNeeded.empty())
        {
            auto iterator = std::find_if(start, end, [orderID](const LevelInfo &o) { return o.orderid_ == orderID; });
            if (iterator == sideNeeded.end())
            {
                std::cout << "\nOrder ID not found at the specified price/time level.\n";
                return;
            }
            if(iterator->quantity_ > expected_to_remove)
            {
                iterator->quantity_ -= expected_to_remove;
                order_iterator->second.first.setQuantity(iterator->quantity_);
                (side == OrderSide::Buy)
                    ? std::cout << "\nRemoved " << expected_to_remove << " piece(s) from OrderID #" << orderID << " (Buy Side).\n"
                    : std::cout << "\nRemoved " << expected_to_remove << " piece(s) from OrderID #" << orderID << " (Ask Side).\n";
            }
            else
            {
                (side == OrderSide::Buy)
                    ? std::cout << "\nRemoved OrderID #" << orderID << " entirely from Bids side. (Missing " << (expected_to_remove - iterator->quantity_) << " piece(s).)\n"
                    : std::cout << "\nRemoved OrderID #" << orderID << " entirely from Asks side. (Missing " << (expected_to_remove - iterator->quantity_) << " piece(s).)\n";
                sideNeeded.erase(iterator);
                if(sideNeeded.empty()) {
                    (side == OrderSide::Buy) ? bids[priceNeeded].erase(timeNeeded) : asks[priceNeeded].erase(timeNeeded);
                }
            }
        }
    }
}


#define Red "\033[1;31m"
#define Green "\033[1;32m"
#define NC "\033[0m"

void OrderBook::CreateOrder(OrderType type, OrderSide orderside, Time_in_Force tif, unsigned long int &curr_oid_b, unsigned long int &curr_oid_s)
{
    bool isCancel = (type == OrderType::Cancel);
    Time time_;
    Price price = 0;
    OrderID oid = 0;
    Quantity quantity = 0;
    if (isCancel)
    {
        if (ids.size() != 0)
        {
            clearScreen();
            ListOwnOrders();
            std::cout << "\nOrder ID = ";
            std::cin >> oid;
            if (ids.find(oid) == ids.end())
            {
                std::cout << Red << "\nThere's no #" << Green << oid << " in the database.\n" << NC;
            }
            else
            {
                std::cout << "\nPrice = ";
                std::cin >> price;
                std::cout << "\nQuantity to remove = ";
                std::cin >> quantity;
                CancelOrder(oid, quantity);
            }
        }
        else
        {
            std::cout << Red << "\nYou don't have any orders that you could cancel right now.\n" << NC;
        }
    }
    else
    {
        clearScreen();
        Display();
        type == OrderType::Market ? (std::cout << "Market ") : (std::cout << "Limit ");
        orderside == OrderSide::Buy ? (std::cout << "Buy ") : (std::cout << "Sell ");
        switch (type)
        {
        case OrderType::Market:
            std::cout << "\nQuantity = ";
            std::cin >> quantity;
            std::cout << "\n";
            switch (orderside)
            {
            case OrderSide::Buy:
            {
                oid = curr_oid_b;
                Order order(type, oid, orderside, price, quantity, tif);
                order.setOrderID(oid);
                order.setQuantity(quantity);
                ++curr_oid_b;
                time_ = getTime();
                BuyMarketOrder(order, time_);
            }
            break;
            case OrderSide::Sell:
            {
                oid = curr_oid_s;
                Order order(type, oid, orderside, price, quantity, tif);
                order.setOrderID(oid);
                order.setQuantity(quantity);
                ++curr_oid_s;
                time_ = getTime();
                SellMarketOrder(order, time_);
            }
            break;
            }
            break;
        case OrderType::Limit:
            std::cout << "\n"
                      << Green << "Buy:" << NC << "Max Limit --" << Red << "Sell:" << NC << "Min Limit";
            std::cout << "\nPrice Limit = ";
            std::cin >> price;
            std::cout << "\nQuantity = ";
            std::cin >> quantity;
            std::cout << "\n";
            switch (orderside)
            {
            case OrderSide::Buy:
            {
                oid = curr_oid_b;
                Order order(type, oid, orderside, price, quantity, tif);
                order.setOrderID(oid);
                order.setQuantity(quantity);
                order.setPrice(price);
                ++curr_oid_b;
                time_ = getTime();
                BuyLimitOrder(order, time_);
            }
            break;
            case OrderSide::Sell:
            {
                oid = curr_oid_s;
                Order order(type, oid, orderside, price, quantity, tif);
                order.setOrderID(oid);
                order.setQuantity(quantity);
                order.setPrice(price);
                ++curr_oid_s;
                time_ = getTime();
                SellLimitOrder(order, time_);
            }
            break;
            }
            break;
        }
    }
}

void OrderBook::ListOwnOrders()
{
    for (auto &[orderid, orderpair] : ids)
    {
        auto order = orderpair.first;
        std::cout << "\n"  << orderid << "       $" << order.getPrice() << "      " << order.getQuantity() << "\n";
    }
}

void OrderBook::Display()
{
    constexpr int priceWidth = 9;
    constexpr int quantityWidth = 6;
    constexpr int timeWidth = 20;
    constexpr int gap = 5;
    constexpr int columnWidth = priceWidth + 2 + quantityWidth + 2 + timeWidth;

    auto format_price = [](Price p) -> std::string
    {
        std::ostringstream oss;
        oss << "$" << std::fixed << std::setprecision(2) << p;
        return oss.str();
    };

    auto get_order_range = [](auto &priceLevel) -> std::pair<Leveliterator, Leveliterator>
    {
        if (!priceLevel.empty() && !priceLevel.begin()->second.empty())
        {
            return {priceLevel.begin()->second.begin(), priceLevel.begin()->second.end()};
        }
        else
        {
            return {{}, {}};
        }
    };

    auto askCurr = asks.begin();
    auto askEnd = asks.end();
    auto buyCurr = bids.begin();
    auto buyEnd = bids.end();

    Leveliterator askOrderCurr, askOrderEnd;
    Leveliterator buyOrderCurr, buyOrderEnd;

    bool hasAsk = (askCurr != askEnd) &&
                  !askCurr->second.empty() &&
                  !askCurr->second.begin()->second.empty();

    bool hasBuy = (buyCurr != buyEnd) &&
                  !buyCurr->second.empty() &&
                  !buyCurr->second.begin()->second.empty();

    if (hasAsk)
    {
        std::tie(askOrderCurr, askOrderEnd) = get_order_range(askCurr->second);
    }
    if (hasBuy)
    {
        std::tie(buyOrderCurr, buyOrderEnd) = get_order_range(buyCurr->second);
    }

    std::uint32_t BuyLevelsPrinted = 0;
    std::uint32_t SellLevelsPrinted = 0;

    std::cout << "\n";
    std::cout << Green << std::left << std::setw(columnWidth) << "Bids (Buy)"
              << std::string(gap, ' ')
              << Red << std::right << std::setw(columnWidth) << "Asks (Sell)"
              << NC << '\n';

    std::cout << std::string(2 * columnWidth + gap, '=') << '\n';

    while (hasBuy || hasAsk)
    {
        // --- Print Buy Side ---
        if (hasBuy && BuyLevelsPrinted < PrintSize)
        {
            std::cout << Green
                      << std::setw(priceWidth) << format_price(buyCurr->first) << "  "
                      << std::setw(quantityWidth) << buyOrderCurr->quantity_ << "  "
                      << std::setw(timeWidth) << PrintTime(FormatToTime(buyOrderCurr->time_.value)) << NC;
            ++buyOrderCurr;
            ++BuyLevelsPrinted;

            if (buyOrderCurr == buyOrderEnd)
            {
                ++buyCurr;
                hasBuy = (buyCurr != buyEnd &&
                          !buyCurr->second.empty() &&
                          !buyCurr->second.begin()->second.empty());

                if (hasBuy)
                {
                    std::tie(buyOrderCurr, buyOrderEnd) = get_order_range(buyCurr->second);
                }
            }
        }
        else
        {
            std::cout << std::string(columnWidth, ' ');
            hasBuy = false;
        }

        std::cout << std::string(gap, ' ');

        // --- Print Ask Side ---
        if (hasAsk && SellLevelsPrinted < PrintSize)
        {
            std::cout << Red
                      << std::setw(priceWidth) << format_price(askCurr->first) << "  "
                      << std::setw(quantityWidth) << askOrderCurr->quantity_ << "  "
                      << std::setw(timeWidth) << PrintTime(FormatToTime(askOrderCurr->time_.value)) << NC;
            ++askOrderCurr;
            ++SellLevelsPrinted;

            if (askOrderCurr == askOrderEnd)
            {
                ++askCurr;
                hasAsk = (askCurr != askEnd &&
                          !askCurr->second.empty() &&
                          !askCurr->second.begin()->second.empty());

                if (hasAsk)
                {
                    std::tie(askOrderCurr, askOrderEnd) = get_order_range(askCurr->second);
                }
            }
        }
        else
        {
            std::cout << std::string(columnWidth, ' ');
            hasAsk = false;
        }

        std::cout << '\n';
    }

    std::cout << std::string(2 * columnWidth + gap, '=') << '\n';
}