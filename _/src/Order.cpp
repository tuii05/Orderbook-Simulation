#include "Order.h"
#include <iostream>


// Prints the necessary informations regarding the order.
template <typename QuantityType, typename IDType, typename PriceType>
void Order<QuantityType, IDType, PriceType>::print() const {
    std::cout << quantity << "@ $" << price << std::endl;
}