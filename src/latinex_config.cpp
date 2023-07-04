#include "latinex_config.h"
#include <cmath>

long double to_long_double(Price in) 
{
    return (double)in / 1000000;
}

Price to_price(long double in) 
{
    return (Price)(std::round(in * 1000000));
}

Balance to_balance(long double in)
{
    return (Balance)(std::round(in * 100));
}

long double from_balance(Balance in)
{
    return (double)in / 100;
}

std::string balance_to_string(Balance in)
{
    std::string tmp = std::to_string(in);
    // put the decimal in the right spot
    if (tmp.size() >= 3) 
    {
        return tmp.substr(0, tmp.size()-2) + "." + tmp.substr(tmp.size()-2);
    }
    else 
    {
        // number is too short, pad to the left
        switch(tmp.size())
        {
            case 1:
                return "0.0" + tmp;
            case 2:
                return "0." + tmp;
        }
    }
    return "0.00"; // should never get hit
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
