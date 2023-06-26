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

/***
 * ITCH prices are 32 bits, with an implied fixed decimal point at a precision of 4. 
 * Ours are 64 bits, so we have to carefully reduce precision
 */
int32_t to_itch_price(Price in) 
{
    return (int32_t)in / 100;
}

Price from_itch_price(int32_t in)
{
    return in * 100;
}
