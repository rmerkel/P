/********************************************************************************************//**
 * @file datum.cc
 *
 * The P Machine's data type
 *
 * class Datum
 ************************************************************************************************/

#include "datum.h"

#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

// public

/********************************************************************************************//**
 ************************************************************************************************/
Datum::Datum() : i{0}, k{Kind::Integer}							{}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(bool value) : b{value}, k{Kind::Boolean}			{}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(char value) : c{value}, k{Kind::Character}			{}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(int value) : i{value}, k{Kind::Integer}			{}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(unsigned value) : u{value}, k{Kind::Integer}		{}

/********************************************************************************************//**
 * @note	Provided for converstion of STL container sizes to unsigned.
 * @param	value	Signed integer value
 ************************************************************************************************/
Datum::Datum(std::size_t value) : u{static_cast<unsigned> (value)}, k{Kind::Integer} {
	assert(numeric_limits<unsigned>::max() > value);
}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(double value) : r{value}, k{Kind::Real}			{}

/********************************************************************************************//**
 * @invariant undefined if kind() == Real
 ************************************************************************************************/
Datum Datum::operator!() const {
	return !b;
}

/********************************************************************************************//**
 * @invariant undefined if kind() == Integer or Real
 ************************************************************************************************/
Datum Datum::operator-() const {
	switch(kind()) {
	case Datum::Integer:	return Datum(-i);
	case Datum::Real:		return Datum(-r);
	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant undefined if kind() != Integer
 ************************************************************************************************/
Datum Datum::operator~() const {
	return Datum(~u);
}

/********************************************************************************************//**
 * @return return my kind
 ************************************************************************************************/
Datum::Kind Datum::kind() const		{	return k;	};

/********************************************************************************************//**
 * @return my boolean value
 ************************************************************************************************/
bool Datum::boolean() const			{	return b;	};

/********************************************************************************************//**
 * @return my character value
 ************************************************************************************************/
char Datum::character() const		{	return c;	};

/********************************************************************************************//**
 * @return my interger value
 ************************************************************************************************/
int Datum::integer() const			{	return i;	};

/********************************************************************************************//**
 * Returns my integer value as a natural.
 * @return my natural value
 ************************************************************************************************/
unsigned Datum::natural() const		{	return u;	};

/********************************************************************************************//**
 * @return my real value
 ************************************************************************************************/
double Datum::real() const			{	return r;	};

/********************************************************************************************//**
 * @return true if my value is numeric
 ************************************************************************************************/
bool Datum::numeric() const	{
	return kind() == Integer || kind() == Real;
}

/********************************************************************************************//**
 * @return true if my value is an ordinal type
 ************************************************************************************************/
bool Datum::ordinal() const {
	return kind() != Real;
}

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
	case Datum::Boolean:	return os << "Boolean";		break;
	case Datum::Character:	return os << "Character";	break;
	case Datum::Integer:	return os << "Integer";		break;
	case Datum::Real:		return os << "Real";		break;
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
	case Datum::Boolean:	return os << value.boolean();
	case Datum::Character:	return os << "'" << value.character() << "'";
	case Datum::Integer:	return os << value.integer();
	case Datum::Real:		return os << value.real();
	default: assert(false); return os << 0;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @invariant	undefined if lhs.kind() != Integer or Real
 * @return lhs + rhs
 ************************************************************************************************/
Datum operator+(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind()) {
		assert(false);
		return Datum(0);
	}

	switch(lhs.kind()) {
	case Datum::Integer:	return Datum(lhs.integer()	+ rhs.integer());
	case Datum::Real:		return Datum(lhs.real()		+ rhs.real());
	default: assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @invariant	undefined if lhs.kind() != Integer or Real
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
	default: assert(false); return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @invariant	undefined if lhs.kind() != Integer or Real
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
	default:
		assert(false);
		return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @invariant	undefined if lhs.kind() != Integer or Real
 * @return lhs / rhs, zero if rhs == 0.
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
		return rhs.real() != 0.0 ? Datum(lhs.real() / rhs.real()) : Datum(0.0);
		break;

	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @invariant	undefined if lhs.kind() != Integer
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

	default:	assert(false);	return Datum(0);
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @return lhs & rhs
 ************************************************************************************************/
Datum operator&(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind() || lhs.kind())
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
	case Datum::Boolean:	return lhs.boolean() 	< rhs.boolean();
	case Datum::Character:	return lhs.character()	< rhs.character();
	case Datum::Integer:	return lhs.integer()	< rhs.integer();
	case Datum::Real:		return lhs.real()		< rhs.real();
	default: assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs <= rhs
 ************************************************************************************************/
bool operator<=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean() 	<= rhs.boolean();
	case Datum::Character:	return lhs.character()	<= rhs.character();
	case Datum::Integer:	return lhs.integer()	<= rhs.integer();
	case Datum::Real:		return lhs.real()		<= rhs.real();
	default: assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs == rhs
 ************************************************************************************************/
bool operator==(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean() 	== rhs.boolean();
	case Datum::Character:	return lhs.character()	== rhs.character();
	case Datum::Integer:	return lhs.integer()	== rhs.integer();
	case Datum::Real:		return lhs.real()		== rhs.real();
	default: assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs >= rhs
 ************************************************************************************************/
bool operator>=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean()	>= rhs.boolean();
	case Datum::Character:	return lhs.character()	>= rhs.character();
	case Datum::Integer:	return lhs.integer()	>= rhs.integer();
	case Datum::Real:		return lhs.real() 		>= rhs.real();
	default: assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs > rhs
 ************************************************************************************************/
bool operator>(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean()	> rhs.boolean();
	case Datum::Character:	return lhs.character()	> rhs.character();
	case Datum::Integer:	return lhs.integer()	> rhs.integer();
	case Datum::Real:		return lhs.real()		> rhs.real();
	default: assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs != rhs
 ************************************************************************************************/
bool operator!=(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean()	!= rhs.boolean();
	case Datum::Character:	return lhs.character()	!= rhs.character();
	case Datum::Integer:	return lhs.integer()	!= rhs.integer();
	case Datum::Real:		return lhs.real() 		!= rhs.real();
	default: assert(false);	return false;
	}
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs && rhs
 ************************************************************************************************/
bool operator&&(const Datum& lhs, const Datum& rhs) {
	return lhs.boolean()	&& rhs.boolean();
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind()
 * @return lhs || rhs
 ************************************************************************************************/
bool operator||(const Datum& lhs, const Datum& rhs) {
	return lhs.boolean()	|| rhs.boolean();
}

