#pragma once

#include "passert.h"

namespace PSupply
{

///////////////////////////////////////////////////////////////////////////////

class SupplyConsumer
{
private:
    int _supply_consumption;

public:
    inline SupplyConsumer()
        :
        SupplyConsumer(0)
    {

    }
    inline explicit SupplyConsumer(int supply_consumption):
        _supply_consumption(supply_consumption)
    {
        PASSERT(supply_consumption >= 0);
        PASSERT(supply_consumption <= 2);
    }
    inline virtual int GetSupplyConsumption() const
    {
        return _supply_consumption;
    }
};

///////////////////////////////////////////////////////////////////////////////

}
