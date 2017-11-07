/********************************************************************************************//**
 * @file datum.cc
 *
 * Pascal-lite Machine's data type
 *
 * A Datum maybe an Interger or Real value.
 ************************************************************************************************/

#include "datum.h"

#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

// public

/********************************************************************************************//**
 * Constructs a integer with value 0
 ************************************************************************************************/
Datum::Datum() : i{0}, k{Kind::Integer}							{}

/********************************************************************************************//**
 * @param	value	Boolean value... converted to 0 (false) or 1 (true)
 ************************************************************************************************/
Datum::Datum(bool value) : b{value}, k{Kind::Boolean}			{}

/********************************************************************************************//**
 * @param	value	Signed integer value
 ************************************************************************************************/
Datum::Datum(int value) : i{value}, k{Kind::Integer}			{}

/********************************************************************************************//**
 * @param	value	unsigned integer value
 ************************************************************************************************/
Datum::Datum(unsigned value) : u{value}, k{Kind::Integer}		{}

/********************************************************************************************//**
 * @note	Provided for converstion of STL container sizes to unsigned.
 * @param	value	Unsigned integer value
 ************************************************************************************************/
Datum::Datum(std::size_t value) : u{static_cast<unsigned> (value)}, k{Kind::Integer} {
	assert(numeric_limits<unsigned>::max() > value);
}

/********************************************************************************************//**
 * @param value Real value
 ************************************************************************************************/
Datum::Datum(double value) : r{value}, k{Kind::Real}			{}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator!() const {
	switch(kind()) {
	case Datum::Integer:	return Datum(!i);
	case Datum::Real:		return Datum(!r);
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator-() const {
	switch(kind()) {
	case Datum::Integer:	return Datum(-i);
	case Datum::Real:		return Datum(-r);
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator~() const {
	switch(kind()) {
	case Datum::Integer:	return Datum(~i);
	case Datum::Real:
		assert(false);
		return Datum(0);

	default:
		assert(false);
		return Datum(0);
	}
}

/********************************************************************************************//**
 * @return return my kind
 ************************************************************************************************/
Datum::Kind Datum::kind() const		{	return k;	};

/********************************************************************************************//**
 * @return my interger value
 ************************************************************************************************/
int Datum::integer() const			{	return i;	};

/********************************************************************************************//**
 * Returns my integer value as a natural.
 * @return my positive value
 ************************************************************************************************/
unsigned Datum::natural() const		{	return u;	};

/********************************************************************************************//**
 * @return my real value
 ************************************************************************************************/
double Datum::real() const			{	return r;	};

/********************************************************************************************//**
 * @return my Boolean value
 ************************************************************************************************/
bool Datum::boolean() const			{	return b;	};

// operators

/********************************************************************************************//**
 * @brief Datum::Kind stream put operator
 *
 * Puts Datum value on os per it's discriminator. 
 *
 * @param	os		Stream to write kind on
 * @param	kind	Value to write on os
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Datum::Kind& kind) {
	switch (kind) {
	case Datum::Integer:	return os << "Integer";	break;
	case Datum::Real:		return os << "Real";	break;
	case Datum::Boolean:	return os << "Boolean";	break;
	default:
		assert(false);
		return os << "Unknown Datum Kind!";
	}
}

/********************************************************************************************//**
 * @brief Datum stream put operator
 *
 * Puts Datum value on os per it's discriminator. 
 *
 * @param	os		Stream to write value on
 * @param	value	Value to write  on os
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Datum& value) {
	switch (value.kind()) {
	case Datum::Integer:	os << value.integer();	break;
	case Datum::Real:		os << value.real();		break;
	case Datum::Boolean:	os << value.boolean();	break;
	default: assert(false); os << 0;
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
	case Datum::Integer:	return Datum(lhs.integer()	+ rhs.integer());
	case Datum::Real:		return Datum(lhs.real()		+ rhs.real());
	case Datum::Boolean:
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
	case Datum::Integer:	return Datum(lhs.integer()	- rhs.integer());
	case Datum::Real:		return Datum(lhs.real()		- rhs.real());
	case Datum::Boolean:
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
	case Datum::Integer:	return Datum(lhs.integer()	* rhs.integer());
	case Datum::Real:		return Datum(lhs.real()		* rhs.real());
	case Datum::Boolean:
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
	case Datum::Integer:
		return rhs.integer() != 0 ? Datum(lhs.integer()	/ rhs.integer()) : Datum(0);
		break;

	case Datum::Real:
		return rhs.real() != 0 ? Datum(lhs.real() / rhs.real()) : Datum(0);
		break;

	case Datum::Boolean:
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
	case Datum::Integer:
		return rhs.integer() != 0 ? Datum(lhs.integer() % rhs.integer()) : Datum(0);
		break;

	case Datum::Real: 
		assert(false);
		return Datum(0);

	case Datum::Boolean:
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @return lhs & rhs
 ************************************************************************************************/
Datum operator&(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind() || lhs.kind() != Datum::Integer)
		assert(false);
	return Datum(lhs.natural() & rhs.natural());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @return lhs | rhs
 ************************************************************************************************/
Datum operator|(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.natural() | rhs.natural());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @return lhs ^ rhs
 ************************************************************************************************/
Datum operator^(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.natural() ^ rhs.natural());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator<<(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.natural() << rhs.natural());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator>>(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.natural() >> rhs.natural());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs < rhs
 ************************************************************************************************/
bool operator<(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	< rhs.integer();
	case Datum::Real:		return lhs.real()		< rhs.real();
	case Datum::Boolean:
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs <= rhs
 ************************************************************************************************/
bool operator<=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	<= rhs.integer();
	case Datum::Real:		return lhs.real()		<= rhs.real();
	case Datum::Boolean:
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs == rhs
 ************************************************************************************************/
bool operator==(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	== rhs.integer();
	case Datum::Real:		return lhs.real()		== rhs.real();
	case Datum::Boolean:	return lhs.boolean() 	== rhs.boolean();
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs >= rhs
 ************************************************************************************************/
bool operator>=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	>= rhs.integer();
	case Datum::Real:		return lhs.real() 		>= rhs.real();
	case Datum::Boolean:
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs > rhs
 ************************************************************************************************/
bool operator>(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	> rhs.integer();
	case Datum::Real:		return lhs.real()		> rhs.real();
	case Datum::Boolean:
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs != rhs
 ************************************************************************************************/
bool operator!=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	!= rhs.integer();
	case Datum::Real:		return lhs.real() 		!= rhs.real();
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs && rhs
 ************************************************************************************************/
bool operator&&(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	&& rhs.integer();
	case Datum::Real:		return lhs.real() 		&& rhs.real();
	case Datum::Boolean:	return lhs.boolean()	&& rhs.boolean();
	default:	assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs || rhs
 ************************************************************************************************/
bool operator||(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Integer:	return lhs.integer()	|| rhs.integer();
	case Datum::Real:		return lhs.real()		|| rhs.real();
	case Datum::Boolean:	return lhs.boolean()	|| rhs.boolean();
	default:	assert(false);	return false;
	}
}

