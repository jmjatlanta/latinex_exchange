#pragma once
#include <cstdint>

/***
 * Quantities and prices are stored as integers. Fractions are kept to a minimum.
 * Internally, they can handle up to 6 decimal points with no loss of precision.
 */

typedef int64_t Price;
typedef int64_t Quantity;

/***
 * Convert a price to a decimal number
 */
long double to_long_double(Price in);

/***
 * Convert a decimal number into a price
 */
Price to_price(long double in);