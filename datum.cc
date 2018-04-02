/********************************************************************************************//**
 * @file datum.cc
 *
 * The P Machine's data type
 *
 * class Datum.
 ************************************************************************************************/

#include "datum.h"

#include <cassert>
#include <cmath>
#include <limits>

#include "results.h"

using namespace std;

// Datum::public

/********************************************************************************************//**
 * Yeilds an Integer zero.
 ************************************************************************************************/
Datum::Datum() : i{0}, k{Kind::Integer}, rng(Subrange::maxRange) {}

/********************************************************************************************//**
 * @param	rhs	New value
 ************************************************************************************************/
Datum::Datum(const Datum& rhs)									{	*this = rhs;	}

/********************************************************************************************//**
 * @param	rhs	New value
 ************************************************************************************************/
Datum::Datum(Datum&& rhs) 										{	*this = rhs;	}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(bool value) : b{value}, k{Kind::Boolean}, rng(false, true) {}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(char value) : c{value}, k{Kind::Character}			{}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(int value) : i{value}, k{Kind::Integer}			{}

/********************************************************************************************//**
 * @throws Result::illegalOp if value is out of range
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(unsigned value) : k{Kind::Integer} {
	if (value > numeric_limits<int>::max())
		throw Result::illegalOp;
	else
		i = value;
}

/********************************************************************************************//**
 * @throws Result::illegalOp if value is out of range
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(size_t value) : k{Kind::Integer} {
	if (value > numeric_limits<int>::max())
		throw Result::illegalOp;
	else
		i = value;
}

/********************************************************************************************//**
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(double value) : r{value}, k{Kind::Real}			{}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator=(const Datum& value) {
	if (this != &value) {
		switch(k = value.kind()) {
		case Kind::Boolean:		b = value.boolean();	break;
		case Kind::Character:	c = value.character();	break;
		case Kind::Integer:		i = value.integer();	break;
		case Kind::Real:		r = value.real();		break;
		default:				assert(false && "unknown Datum::Kind!");
		}
		rng = value.rng;
	}

	return *this;
}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator=(Datum&& value) {
	if (this != &value) {
		switch(k = value.kind()) {
		case Kind::Boolean:		b = value.boolean();	break;
		case Kind::Character:	c = value.character();	break;
		case Kind::Integer:		i = value.integer();	break;
		case Kind::Real:		r = value.real();		break;
		default:				assert(false && "unknown Datum::Kind!");
		}
		rng = value.rng;
	}

	// note really neded, but put value into default constructed state
	value.k = Kind::Integer;
	value.i = 0;

	return *this;
}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this, which is now a Boolean
 ************************************************************************************************/
Datum& Datum::operator=(bool value)			{	return *this = Datum(value);	}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this, which is now a Character
 ************************************************************************************************/
Datum& Datum::operator=(char value)			{	return *this = Datum(value);	}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this, which is now an Integer
 ************************************************************************************************/
Datum& Datum::operator=(int value)			{	return *this = Datum(value);	}

/********************************************************************************************//**
 * @throws	Result::illegalOp if value is out of range
 * @param	value	new value, and type
 * @return	*this, which is now an Integer
 ************************************************************************************************/
Datum& Datum::operator=(unsigned value) {
	if (value > numeric_limits<int>::max())
		throw Result::illegalOp;

	return *this = Datum(value);
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if value is out of range
 * @param	value	new value, and type
 * @return	*this, which is now an Integer
 ************************************************************************************************/
Datum& Datum::operator=(size_t value) {
	if (value > numeric_limits<int>::max())
		throw Result::illegalOp;

	return *this = Datum(value);
}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this, which is now a Real
 ************************************************************************************************/
Datum& Datum::operator=(double value)		{	return *this = Datum(value);	}

/********************************************************************************************//**
 * @return	modified copy of this
 ************************************************************************************************/
Datum Datum::operator!() const				{	return Datum(!b);	}

/********************************************************************************************//**
 * @throws	Result::illegalOp if illegal type for unary negation
 * @return	modified copy of this
 ************************************************************************************************/
Datum Datum::operator-() const {
	switch(kind()) {
	case Datum::Integer:	return Datum(-i);
	case Datum::Real:		return Datum(-r);
	default: throw Result::illegalOp;
	}
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if illegal type for unary bitwise not
 * @return	modified copy of this
 ************************************************************************************************/
Datum Datum::operator~() const {
	if (kind() != Kind::Integer)
		throw Result::illegalOp;

	return Datum(~i);
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator++() {
	switch(kind()) {
	case Kind::Integer:	++i;	break;
	case Kind::Real:	++r;	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator--() {
	switch(kind()) {
	case Kind::Integer:	--i;	break;
	case Kind::Real:	--r;	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @return	Copy of *this before the operator
 ************************************************************************************************/
Datum Datum::operator++(int) {
	Datum prev= *this;

	switch(kind()) {
	case Kind::Integer:	++i;	break;
	case Kind::Real:	++r;	break;
	default: throw Result::illegalOp;
	}

	return prev;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @return	Copy of *this before the operator
 ************************************************************************************************/
Datum Datum::operator--(int) {
	Datum prev= *this;

	switch(kind()) {
	case Kind::Integer:	--i;	break;
	case Kind::Real:	--r;	break;
	default: throw Result::illegalOp;
	}

	return prev;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator+=(const Datum& rhs) {
	if (kind() != rhs.kind())
		throw Result::illegalOp;

	switch(kind()) {
	case Kind::Integer:	i += rhs.integer();	break;
	case Kind::Real:	r += rhs.real();	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator-=(const Datum& rhs) {
	if (kind() != rhs.kind())
		throw Result::illegalOp;

	switch(kind()) {
	case Kind::Integer:	i -= rhs.integer();	break;
	case Kind::Real:	r -= rhs.real();	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator*=(const Datum& rhs) {
	if (kind() != rhs.kind())
		throw Result::illegalOp;

	switch(kind()) {
	case Kind::Integer:	i *= rhs.integer();	break;
	case Kind::Real:	r *= rhs.real();	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::divideByZero if rhs.zero().
 * @throws	Result::illegalOp if type is a non-numeric
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator/=(const Datum& rhs) {
	if (kind() != rhs.kind())
		throw Result::illegalOp;

	else if (rhs.zero())
		throw Result::divideByZero;
	
	switch(kind()) {
	case Kind::Integer:	i /= rhs.integer();	break;
	case Kind::Real:	r /= rhs.real();	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::divideByZero if rhs.zero().
 * @throws	Result::illegalOp if type is a non-numeric
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator%=(const Datum& rhs) {
	if (kind() != rhs.kind())
		throw Result::illegalOp;

	else if (rhs.zero())
		throw Result::divideByZero;

	switch(kind()) {
	case Kind::Integer:	i %= rhs.integer();				break;
	case Kind::Real:	r = remainder(r,rhs.real());	break;
	default: throw Result::illegalOp;
	}

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric or negative
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator&=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw Result::illegalOp;

	else if (i < 0 || rhs.integer() < 0)
		throw Result::illegalOp;

	i &= rhs.natural();

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric or negative
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator|=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw Result::illegalOp;

	else if (i < 0 || rhs.integer() < 0)
		throw Result::illegalOp;

	i |= rhs.natural();

	return *this;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is a non-numeric or negative
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator^=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw Result::illegalOp;

	else if (i < 0 || rhs.integer() < 0)
		throw Result::illegalOp;

	i ^= rhs.natural();

	return *this;
}

/********************************************************************************************//**
 * @throw	Result::illegalOp if kind() or rhs.kind() != Datum::Integer or are negative
 * @param	rhs The right-hand-side
 *
 * @return lhs << rhs
 ************************************************************************************************/
Datum& Datum::operator>>=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw Result::illegalOp;

	else if (integer() < 0 || rhs.integer() < 0)
		throw Result::illegalOp;

	i >>= rhs.natural();

	return *this;
}

/********************************************************************************************//**
 * @throw	Result::illegalOp if kind() or rhs.kind() != Datum::Integer or are negative
 * @param	rhs The right-hand-side
 *
 * @return lhs << rhs
 ************************************************************************************************/
Datum& Datum::operator<<=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw Result::illegalOp;

	else if (integer() < 0 || rhs.integer() < 0)
		throw Result::illegalOp;

	i <<= rhs.natural();

	return *this;
}

/********************************************************************************************//**
 * @return return my kind
 ************************************************************************************************/
Datum::Kind Datum::kind() const				{	return k;	};

/********************************************************************************************//**
 * @throws	Result::illegalOp if type isn't boolean
 * @return my boolean value
 ************************************************************************************************/
bool Datum::boolean() const {
	if (kind() != Datum::Boolean)
		throw Result::illegalOp;

	return b;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type isn't a character
 * @return my character value
 ************************************************************************************************/
char Datum::character() const {
	if (kind() != Datum::Character)
		throw Result::illegalOp;

	return c;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type isn't an integer
 * @return my interger value
 ************************************************************************************************/
int Datum::integer() const {
	if (kind() != Datum::Integer)
		throw Result::illegalOp;

	return i;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if my value is negative
 * @return my interger value
 ************************************************************************************************/
unsigned Datum::natural() const {
	if (i < 0)
		throw Result::illegalOp;

	return i;
}

/********************************************************************************************//**
 * @throws	Result::illegalOp if type is non-numeric
 * @note will convert an integer to a real.
 * @return my real value
 ************************************************************************************************/
double Datum::real() const {
	if (kind() == Datum::Real)
		return r;

	else if (kind() == Datum::Integer)
		return i * 1.0;

	else
		throw Result::illegalOp;
}

/********************************************************************************************//**
 * @return true if my value is numeric
 ************************************************************************************************/
bool Datum::numeric() const					{	return kind() == Integer || kind() == Real;	}

/********************************************************************************************//**
 * @return true if my value is an ordinal type
 ************************************************************************************************/
bool Datum::ordinal() const					{	return kind() != Real;	}

/********************************************************************************************//**
 * @return true if my value is equal to zero
 ************************************************************************************************/
bool Datum::zero() const {
	switch(kind()) {
		case Kind::Integer:	return i == 0;
		case Kind::Real:	return r == 0.0;

		default:			return false;
	}
}

/********************************************************************************************//**
 * @return	My range
 ************************************************************************************************/
const Subrange& Datum::range() const		{	return rng;	}

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
 * @invariant	undefined if parametes aren't numeric
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs + rhs
 ************************************************************************************************/
Datum operator+(const Datum& lhs, const Datum& rhs) {
	Datum sum = lhs;
	return sum += rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if parameters aren't numeric
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs - rhs
 ************************************************************************************************/
Datum operator-(const Datum& lhs, const Datum& rhs) {
	Datum diff = lhs;
	return diff -= rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if parameters aren't numeric
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs * rhs
 ************************************************************************************************/
Datum operator*(const Datum& lhs, const Datum& rhs) {
	Datum prod = lhs;
	return prod *= rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if parameters aren't numeric
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs / rhs, zero if rhs == 0.
 ************************************************************************************************/
Datum operator/(const Datum& lhs, const Datum& rhs) {
	Datum quot = lhs;
	return quot /= rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if parameters aren't numeric
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs % rhs
 ************************************************************************************************/
Datum operator%(const Datum& lhs, const Datum& rhs) {
	Datum rem = lhs;
	return rem %= rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs & rhs
 ************************************************************************************************/
Datum operator&(const Datum& lhs, const Datum& rhs) {
	Datum result = lhs;
	return result &= rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs | rhs
 ************************************************************************************************/
Datum operator|(const Datum& lhs, const Datum& rhs) {
	Datum result = lhs;
	return result |= rhs;
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs ^ rhs
 ************************************************************************************************/
Datum operator^(const Datum& lhs, const Datum& rhs) {
	Datum result = lhs;
	return result ^= rhs;
}

/********************************************************************************************//**
 * @throw	Result::illegalOp if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator<<(const Datum& lhs, const Datum& rhs) {
	Datum result = lhs;
	return result <<= rhs;
}

/********************************************************************************************//**
 * @throw	Result::illegalOp if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator>>(const Datum& lhs, const Datum& rhs) {
	Datum result = lhs;
	return result >>= rhs;
}

/********************************************************************************************//**
 * @note	use std::rel_ops for <=, !=, >, >=
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs < rhs
 ************************************************************************************************/
bool operator<(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		return false;

	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean()	< rhs.boolean();
	case Datum::Character:	return lhs.character()	< rhs.character();
	case Datum::Integer:	return lhs.integer()	< rhs.integer();
	case Datum::Real:		return lhs.real()		< rhs.real();
	default:				assert(false); return false;
	}

}

/********************************************************************************************//**
 * @note	use std::rel_ops for <=, !=, >, >=
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs == rhs
 ************************************************************************************************/
bool operator==(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		return false;

	switch(lhs.kind()) {
	case Datum::Boolean:	return lhs.boolean()	== rhs.boolean();
	case Datum::Character:	return lhs.character()	== rhs.character();
	case Datum::Integer:	return lhs.integer()	== rhs.integer();
	case Datum::Real:		return lhs.real()		== rhs.real();
	default:				assert(false); return false;
	}

}

/********************************************************************************************//**
 * @invariant	undefined if parameters are Boolean
 * @return lhs && rhs
 ************************************************************************************************/
bool operator&&(const Datum& lhs, const Datum& rhs) {
	return lhs.boolean() && rhs.boolean();
}

/********************************************************************************************//**
 * @invariant	undefined if parameters are Boolean
 * @return lhs || rhs
 ************************************************************************************************/
bool operator||(const Datum& lhs, const Datum& rhs) {
	return lhs.boolean() || rhs.boolean();
}

