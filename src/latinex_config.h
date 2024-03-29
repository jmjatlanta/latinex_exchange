#pragma once
#include <cstdint>
#include <string>

/***
 * Quantities and prices are stored as integers. Fractions are kept to a minimum.
 * Internally, they can handle up to 6 decimal points with no loss of precision.
 */

typedef int64_t Price;
typedef int64_t Quantity;
typedef int64_t Balance;
/***
 * Convert a price to a decimal number
 */
long double to_long_double(Price in);

/***
 * Convert a decimal number into a price
 */
Price to_price(long double in);

Balance to_balance(long double in);
long double from_balance(Balance in);
std::string balance_to_string(Balance in);
/***
 * ITCH prices are 32 bits, with an implied fixed decimal point at a precision of 4. 
 * Ours are 64 bits, so we have to carefully reduce precision
 */
int32_t to_itch_price(Price in);

Price from_itch_price(int32_t in);