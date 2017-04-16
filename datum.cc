/** @file datum.cc
 *
 *  PL0/C Machine's data type
 *
 *  A Datum maybe an Interger or Real value, or Unsigned (for data addresses)
 */

#include "datum.h"

#include <cassert>
#include <cmath>
#include <limits>

using namespace std;
using namespace pl0c;

/// @param	value	Signed integer value
Datum::Datum(pl0c::Integer value)	: i{value}	{}

/// @param	value	Unsigned integer value
Datum::Datum(pl0c::Unsigned value)	: u{value}	{}

/**
 * @note	Provided for converstion of STL container sizes to Unsigned.
 * @param	value	Unsigned integer value
 */
Datum::Datum(std::size_t value)	{
	assert(numeric_limits<Unsigned>::max() > value);
	u = static_cast<Integer> (value);
}

/// @param	value	Real value
Datum::Datum(pl0c::Real value)	: r{value}		{}

