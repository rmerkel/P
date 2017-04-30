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

/// Constructs a integer with value 0
Datum::Datum() : i{0}, k{Kind::Integer}						{}

/// @param	value	Signed integer value
Datum::Datum(Integer value) : i{value}, k{Kind::Integer}	{}

/// @param	value	Unsigned integer value
Datum::Datum(Unsigned value) : u{value}, k{Kind::Unsigned}	{}

/**
 * @note	Provided for converstion of STL container sizes to Unsigned.
 * @param	value	Unsigned integer value
 */
Datum::Datum(std::size_t value) : u{static_cast<Unsigned> (value)}, k{Kind::Unsigned}	{
	assert(numeric_limits<Unsigned>::max() > value);
}

/// @param	value	Real value
Datum::Datum(Real value) : r{value}, k{Kind::Real}			{}

// operators

/** 
 * Puts Datum value per it's discriminator. 
 * @param	os	Stream to write d's value to 
 * @param	d	Datum whose value to write 
 * @return	os 
 */
ostream& operator<<(std::ostream& os, const Datum& d) {
	switch (d.k) {
	case Datum::Kind::Integer:	os << d.i;	break;
	case Datum::Kind::Unsigned:	os << d.u;	break;
	case Datum::Kind::Real:		os << d.r;	break;
	default:	assert(false);  os << 0;
	}

	return os;
}
