#include "latinex_config.h"
#include <cmath>

long double to_long_double(Price in) 
{
    return (double)in / 1000000;
}

Price to_price(long double in) 
{
    long double tmp = in * 1000000;
    long double rounded = std::round(tmp);
    return (Price)(std::round(in * 1000000));
}