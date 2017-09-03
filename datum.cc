/********************************************************************************************//**
 * @file datum.cc
 *
 *  Pascal-Lite Machine's data type
 *
 *  A Datum maybe an Interger or Real value, or Unsigned (for data addresses)
 ************************************************************************************************/

#include "datum.h"

#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

// public static

/********************************************************************************************//**
 * @param k	Kind whose name to return
 ************************************************************************************************/
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

// public

/********************************************************************************************//**
 * Constructs a integer with value 0
 ************************************************************************************************/
Datum::Datum() : i{0}, k{Kind::Integer}						{}

/********************************************************************************************//**
 * @param	value	Signed integer value
 ************************************************************************************************/
Datum::Datum(Integer value) : i{value}, k{Kind::Integer}	{}

/********************************************************************************************//**
 * @param	value	Unsigned integer value
 ************************************************************************************************/
Datum::Datum(Unsigned value) : u{value}, k{Kind::Unsigned}	{}

/********************************************************************************************//**
 * @note	Provided for converstion of STL container sizes to Unsigned.
 * @param	value	Unsigned integer value
 ************************************************************************************************/
Datum::Datum(std::size_t value) : u{static_cast<Unsigned> (value)}, k{Kind::Unsigned}	{
	assert(numeric_limits<Unsigned>::max() > value);
}

/********************************************************************************************//**
 ************************************************************************************************/
Datum::Datum(Real value) : r{value}, k{Kind::Real}			{}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator!() const {
	switch(kind()) {
	case Datum::Kind::Integer:	return Datum(!i);
	case Datum::Kind::Unsigned:	return Datum(!u);
	case Datum::Kind::Real:		return Datum(!r);
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator-() const {
	switch(kind()) {
	case Datum::Kind::Integer:	return Datum(-i);
	case Datum::Kind::Unsigned:	return Datum(-u);
	case Datum::Kind::Real:		return Datum(-r);
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator~() const {
	switch(kind()) {
	case Datum::Kind::Integer:	return Datum(~i);
	case Datum::Kind::Unsigned:	return Datum(~u);
	case Datum::Kind::Real:
		assert(false);
		return Datum(0);

	default:
		assert(false);
		return Datum(0);
	}
}

// operators

/********************************************************************************************//**
 * @brief Datum stream put operator
 *
 * Puts Datum value on os per it's discriminator. 
 *
 * @param	os	Stream to write d's value to 
 * @param	d	Datum whose value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Datum& d) {
	switch (d.kind()) {
	case Datum::Kind::Integer:	os << d.integer();	break;
	case Datum::Kind::Unsigned:	os << d.uinteger();	break;
	case Datum::Kind::Real:		os << d.real();	break;
	default:	assert(false);  os << 0;
	}

	return os;
}

/********************************************************************************************//**
 * @invariant	undefined if l.kind() != r.kind()
 * @return l + r
 ************************************************************************************************/
Datum operator+(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind()) {
		assert(false);
		return Datum(0);
	}

	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	+ rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger()	+ rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		+ rhs.real());
	default:
		assert(false);
		return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs - rhs
 ************************************************************************************************/
Datum operator-(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind()) {
		assert(false);
		return Datum(0);
	}

	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	- rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger()	- rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		- rhs.real());
	default:
		assert(false);
		return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs * rhs
 ************************************************************************************************/
Datum operator*(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind()) {
		assert(false);
		return Datum(0);
	}

	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	* rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger()	* rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		* rhs.real());
	default:
		assert(false);
		return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs / rhs
 ************************************************************************************************/
Datum operator/(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind()) {
		assert(false);
		return Datum(0);
	}

	switch(lhs.kind()) {
	case Datum::Kind::Integer:
		return rhs.integer() != 0 ? Datum(lhs.integer()	/ rhs.integer()) : Datum(0);
		break;

	case Datum::Kind::Unsigned:
		return rhs.uinteger() != 0 ? Datum(lhs.uinteger() / rhs.uinteger()) : Datum(0);
		break;

	case Datum::Kind::Real:
		return rhs.real() != 0 ? Datum(lhs.real() / rhs.real()) : Datum(0);
		break;

	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs % rhs
 ************************************************************************************************/
Datum operator%(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind()) {
		assert(false);
		return Datum(0);
	}

	switch(lhs.kind()) {
	case Datum::Kind::Integer:
		return rhs.integer() != 0 ? Datum(lhs.integer() % rhs.integer()) : Datum(0);
		break;

	case Datum::Kind::Unsigned:
		return rhs.uinteger() != 0 ? Datum(lhs.uinteger() % rhs.uinteger()) : Datum(0);
		break;

	case Datum::Kind::Real: 
		assert(false);
		return Datum(0);

	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Kind::Unsigned
 * @return lhs & rhs
 ************************************************************************************************/
Datum operator&(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind() || lhs.kind() != Datum::Kind::Unsigned)
		assert(false);
	return Datum(lhs.uinteger() & rhs.uinteger());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Kind::Unsigned
 * @return lhs | rhs
 ************************************************************************************************/
Datum operator|(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.uinteger() | rhs.uinteger());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Kind::Unsigned
 * @return lhs ^ rhs
 ************************************************************************************************/
Datum operator^(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.uinteger() ^ rhs.uinteger());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Kind::Unsigned
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator<<(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.uinteger() << rhs.uinteger());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Kind::Unsigned
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator>>(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.uinteger() >> rhs.uinteger());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs < rhs
 ************************************************************************************************/
Datum operator<(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	< rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger()	< rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		< rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs <= rhs
 ************************************************************************************************/
Datum operator<=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	<= rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() <= rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		<= rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs == rhs
 ************************************************************************************************/
Datum operator==(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	== rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() == rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		== rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs >= rhs
 ************************************************************************************************/
Datum operator>=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	>= rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() >= rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real() 	>= rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs > rhs
 ************************************************************************************************/
Datum operator>(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	> rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() > rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		> rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs != rhs
 ************************************************************************************************/
Datum operator!=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	!= rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() != rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real() 	!= rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs && rhs
 ************************************************************************************************/
Datum operator&&(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	&& rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() && rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real() 	&& rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs || rhs
 ************************************************************************************************/
Datum operator||(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Kind::Integer:	return Datum(lhs.integer()	|| rhs.integer());
	case Datum::Kind::Unsigned:	return Datum(lhs.uinteger() || rhs.uinteger());
	case Datum::Kind::Real:		return Datum(lhs.real()		|| rhs.real());
	default:	assert(false);	return Datum(0);
	}
}

