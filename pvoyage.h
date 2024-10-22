#pragma once

#include "passert.h"

namespace PVoyage
{

///////////////////////////////////////////////////////////////////////////////

enum class VoyageLength : uint8_t
{
    SHORT = 1,
    MEDIUM,
    LONG
};

///////////////////////////////////////////////////////////////////////////////

class Voyage
{
private:
    int _planned_months;
    int _remaining_months;

public:
    Voyage()
        :
        Voyage(VoyageLength::SHORT)
    {
    }
    explicit Voyage(const VoyageLength &voyage_length)
    {
        switch(voyage_length)
        {
        case VoyageLength::SHORT:
            _planned_months = 10;
            break;

        case VoyageLength::MEDIUM:
            _planned_months = 20;
            break;

        case VoyageLength::LONG:
            _planned_months = 30;
            break;

        default:
            PASSERT(0);
            break;
        };
        _remaining_months = _planned_months;
    }
    inline int GetPlannedMonths() const
    {
        return _planned_months;
    }
    inline int GetRemainingMonths() const
    {
        return _remaining_months;
    }
    inline int GetPassedMonths() const
    {
        return GetPlannedMonths() - GetRemainingMonths();
    }
    inline void SpendMonths(int months)
    {
        PASSERT(months > 0);
        _remaining_months -= months;
    }
};

///////////////////////////////////////////////////////////////////////////////

}
