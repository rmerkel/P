/** @file datum.cc
 *
 *  PL0/C Machine's data type implementation.
 */

#include "datum.h"

#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

/********************************************************************************
 * public static
 ********************************************************************************/

/// @param k	Kind whose name to return
string Datum::toString(Kind k) {
	switch (k) {
	case Datum::Kind::Integer:	return "Integer";	break;
	case Datum::Kind::Unsigned:	return "Unsined";	break;
	case Datum::Kind::Real:		return "Real";		break;
	default:
		assert(false);
		return "Unknown Datum Kind!";
	}
}

/********************************************************************************
 * public
 ********************************************************************************/

/// Constructs a integer with value 0
Datum::Datum() : i{0}, k{Kind::Integer}						{}

/// @param	value	Signed integer value
Datum::Datum(Integer value) : i{value}, k{Kind::Integer}	{}

/// @param	value	Unsigned integer value
Datum::Datum(Unsigned value) : u{value}, k{Kind::Unsigned}	{}

/** 
 * @invariant value must fit into a Datum::Integer. 
 * @note	Provided for converstion of STL container sizes to Unsigned. 
 * @param	value	Unsigned integer value
 */
Datum::Datum(std::size_t value) : u{static_cast<Unsigned> (value)}, k{Kind::Unsigned} {
	assert(numeric_limits<Unsigned>::max() > value);
}

/// @param	value	Unsigned integer value
Datum::Datum(Real value) : r(value), k(Kind::Real)			{}

Datum::Integer Datum::integer() const {
	switch (kind()) {
	case Kind::Integer:
	case Kind::Unsigned:	
		return i;

	case Kind::Real:
		return static_cast<Integer> (round(r));

	default:
		assert(false);
		return 0;
	}
}

Datum::Unsigned Datum::uinteger() const {
	switch (kind()) {
	case Datum::Kind::Integer:
	case Datum::Kind::Unsigned:
		return u;
		break;

	case Datum::Kind::Real:
		return static_cast<Unsigned> (r + 0.5);

	default:	
		assert(false);
		return 0u;
	}
}

Datum::Real Datum::real() const {
	switch (kind()) {
	case Datum::Kind::Integer:
		return i * 1.0;

	case Datum::Kind::Unsigned:
		return i + 1.0;

	case Datum::Kind::Real:
		return r;

	default:
		assert(false);
		return 0.0;
	}
}

/********************************************************************************
 * Assignment
 ********************************************************************************/

/**
 * @param value New signed integer value 
 * @return *this 
 */
Datum& Datum::operator=(Integer value) 	{	
	k = Kind::Integer;
	i = value;
	return *this;
}


/**
 * @param value New unsigned integer value 
 * @return *this 
 */
Datum& Datum::operator=(Unsigned value) {
	k = Kind::Unsigned;
	u = value;
	return *this;
}

/** 
 * @invariant value must fit into a Datum::Integer. 
 * @note	Provided for converstion of STL container sizes to Unsigned.  
 * @param	value New unsigned integer value 
 * @return 	*this 
 */
Datum& Datum::operator=(std::size_t value) {
	assert(numeric_limits<Unsigned>::max() > value);
	k = Kind::Unsigned;
	u = value;
	return *this;
}


/**
 * @param value New real value 
 * @return *this 
 */
Datum& Datum::operator=(Real value) {
	k = Kind::Real;
	r = value;
	return *this;
}

/********************************************************************************
 * unary operators
 ********************************************************************************/

Datum operator!(const Datum& rhs) {
	if (Datum::Kind::Real == rhs.kind())
		return	Datum(!rhs.real());
	else if (Datum::Kind::Unsigned == rhs.kind())
		return	Datum(!rhs.uinteger());
	else
		return	Datum(!rhs.integer());
}

Datum operator-(const Datum& rhs) {
	if (Datum::Kind::Real == rhs.kind())
		return	Datum(-rhs.real());
	else if (Datum::Kind::Unsigned == rhs.kind())
		return	Datum(-rhs.uinteger());
	else
		return	Datum(-rhs.integer());
}

/// @return a unsigned Datum
Datum operator~(const Datum& rhs) {
	return	Datum(~rhs.uinteger());
}

/********************************************************************************
 * binary operators
 ********************************************************************************/

Datum operator+(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return Datum(lhs.real()		+ rhs.real());

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return Datum(lhs.uinteger() + rhs.uinteger());

	else
		return Datum(lhs.integer()	+ rhs.integer());
}

Datum operator-(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return Datum(lhs.real()		- rhs.real());

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return Datum(lhs.uinteger() - rhs.uinteger());

	else
		return Datum(lhs.integer()	- rhs.integer());
}

Datum operator*(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return Datum(lhs.real()		* rhs.real());

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return Datum(lhs.uinteger() * rhs.uinteger());

	else
		return Datum(lhs.integer()	* rhs.integer());
}

/// @note division by zero is undefined
Datum operator/(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return Datum(lhs.real()		/ rhs.real());

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return Datum(lhs.uinteger() / rhs.uinteger());

	else
		return Datum(lhs.integer()	/ rhs.integer());
}

/// @note division by zero is undefined
Datum operator%(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return Datum(remainder(lhs.real(), rhs.real()));

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return Datum(lhs.uinteger() % rhs.uinteger());

	else
		return Datum(lhs.integer()	% rhs.integer());
}

/// @return a unsigned Datum
Datum operator|(const Datum& lhs, const Datum& rhs) {
	return Datum(lhs.uinteger() | rhs.uinteger());
}

/// @return a unsigned Datum
Datum operator&(const Datum& lhs, const Datum& rhs) {
	return Datum(lhs.uinteger() & rhs.uinteger());
}

/// @return a unsigned Datum
Datum operator^(const Datum& lhs, const Datum& rhs) {
	return Datum(lhs.uinteger() ^ rhs.uinteger());
}

/// @return a unsigned Datum
Datum operator<<(const Datum& lhs, const Datum& rhs) {
	return Datum(lhs.uinteger() << rhs.integer());
}

/// Return lhs right shifted by rhs
Datum operator>>(const Datum& lhs, const Datum& rhs) {
	return Datum(lhs.uinteger() >> rhs.integer());
}

/********************************************************************************
 * logical operators
 ********************************************************************************/

/// Return true if lhs is less than rhs
bool operator<(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return lhs.real()		< rhs.real();

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return lhs.uinteger()	< rhs.uinteger();

	else
		return lhs.integer()	< rhs.integer();
}

// Return true if lhs is equal to rhs
bool operator==(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return lhs.real()		== rhs.real();

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return lhs.uinteger()	== rhs.uinteger();

	else
		return lhs.integer()	== rhs.integer();
}

bool operator||(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return lhs.real() || rhs.real();

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return lhs.uinteger() || rhs.uinteger();

	else
		return lhs.integer() || rhs.integer();
}

bool operator&&(const Datum& lhs, const Datum& rhs) {
	if (Datum::Kind::Real == lhs.kind() || Datum::Kind::Real == rhs.kind())
		return lhs.real() && rhs.real();

	else if (Datum::Kind::Unsigned == lhs.kind() || Datum::Kind::Unsigned == rhs.kind())
		return lhs.uinteger() && rhs.uinteger();

	else
		return lhs.integer() && rhs.integer();
}

/********************************************************************************
 * stream operators
 ********************************************************************************/

/** 
 * Puts Datum value per it's discriminator. 
 * @param	os	Stream to write d's value to 
 * @param	d	Datum whose value to write 
 * @return	os 
 */
ostream& operator<<(std::ostream& os, const Datum& d) {
	switch (d.kind()) {
	case Datum::Kind::Integer:	os << d.integer();	break;
	case Datum::Kind::Unsigned:	os << d.uinteger();	break;
	case Datum::Kind::Real:		os << d.real();		break;
	default:	assert(false);  os << 0;
	}

	return os;
}
