
#include "stdafx.h"
#include "tween.h"

#include <float.h>
#include <math.h>

// static
double Tween::CalculateValue(Tween::Type type, double state)
{
	ASSERT(state >= 0 && state <= 1);

    switch(type)
    {
    case EASE_IN:
        return pow(state, 2);

    case EASE_IN_OUT:
        if(state < 0.5)
        {
            return pow(state * 2, 2) / 2.0;
        }
        return 1.0 - (pow((state - 1.0) * 2, 2) / 2.0);

    case FAST_IN_OUT:
        return (pow(state - 0.5, 3) + 0.125) / 0.25;

    case LINEAR:
        return state;

    case EASE_OUT_SNAP:
        state = 0.95 * (1.0 - pow(1.0 - state, 2));
        return state;

    case EASE_OUT:
        return 1.0 - pow(1.0 - state, 2);

    case ZERO:
        return 0;
    }

    ASSERT(FALSE);
    return state;
}

// static
double Tween::ValueBetween(double value, double start, double target)
{
    return start + (target - start) * value;
}

// static
int Tween::ValueBetween(double value, int start, int target)
{
    if(start == target)
    {
        return start;
    }
    double delta = static_cast<double>(target - start);
    if(delta < 0)
    {
        delta--;
    }
    else
    {
        delta++;
    }
    return start + static_cast<int>(value * _nextafter(delta, 0));
}