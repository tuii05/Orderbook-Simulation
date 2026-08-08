#pragma once

template <typename QuantityType, typename IDType, typename PriceType>
struct ExecutionResult
{
    QuantityType remainingQuantity{};
    std::size_t matchedLevels{};
    PriceType totalValue{};
    PriceType lastPrice{};
    IDType orderID{};
};