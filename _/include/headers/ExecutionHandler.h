#pragma once

#include "DataTypes.h"

#include <array>
#include <iostream>
#include <type_traits>


template<typename QuantityType, typename IDType, typename PriceType>
class ExecutionHandler {
private:
    using T = DataTypes<QuantityType, IDType, PriceType>;

public:
    using OrderT = typename T::OrderT;
    using ExecutionT = typename T::ExecutionT;
    using ExecutionResultT = typename T::ExecutionResultT;
    using Quantity = typename T::Quantity;


public:
    ExecutionHandler() = default;

    ExecutionHandler(const ExecutionHandler&) = delete;
    ExecutionHandler& operator=(const ExecutionHandler&) = delete;

    ExecutionHandler(ExecutionHandler&&) = delete;
    ExecutionHandler& operator=(ExecutionHandler&&) = delete;

public:
    bool MarketHandleLogic(
        const PriceType& levelPrice,
        const ExecutionT& execution
    ) const;

    bool LimitHandleLogic(
        const PriceType& levelPrice,
        const ExecutionT& execution
    ) const;

    bool checkLogic(
        const PriceType& levelPrice,
        const ExecutionT& execution
    ) const;

private:
    using typeHandler = bool (ExecutionHandler::*)(
            const PriceType&,
            const ExecutionT&
    ) const;

    std::array<typeHandler, 2> handleLogicFunction = {
        &ExecutionHandler::MarketHandleLogic,
        &ExecutionHandler::LimitHandleLogic
    };


private:
    template<typename BookDataType>
    void handleCreate(const OrderT& order, BookDataType& data);

    template<typename BookDataType>
    void handleCancel(
        const OrderT& order,
        BookDataType& data
    );

public:
    template<typename BookDataType>
    ExecutionResultT handleExecution(
        ExecutionT& execution,
        BookDataType& data
    );


public:
    ExecutionResultT handleGTC(
        const ExecutionResultT& result,
        const ExecutionT& execution
    );

    ExecutionResultT handleFK(
        const ExecutionResultT& result,
        const ExecutionT& execution
    );


private:
    using tifHandler =
        ExecutionResultT (ExecutionHandler::*)(
            const ExecutionResultT&,
            const ExecutionT&
        );

    std::array<tifHandler, 2> handleTifFunction =
    {
        &ExecutionHandler::handleGTC,
        &ExecutionHandler::handleFK
    };

public:
    ExecutionResultT handleTIF(
        const ExecutionResultT& result,
        const ExecutionT& execution
    );
};
