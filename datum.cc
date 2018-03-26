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

using namespace std;

// Datum::public

/********************************************************************************************//**
 * Yeilds an Integer zero.
 ************************************************************************************************/
Datum::Datum() : i{0}, k{Kind::Integer}							{}

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
 * Throws IllegalOp if value is out of range
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(unsigned value) : k{Kind::Integer} {
	if (value > numeric_limits<int>::max())
		throw IllegalOp("Attempt to construct an integer with an out-of-range value");
	else
		i = value;
}

/********************************************************************************************//**
 * Throws utOfRange if value is out of range
 * @param	value	initial value
 ************************************************************************************************/
Datum::Datum(size_t value) : k{Kind::Integer} {
	if (value > numeric_limits<int>::max())
		throw IllegalOp("Attempt to construct an integer with an out-of-range value");
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
 * @param	value	new value, and type
 * @return	*this, which is now an Integer
 ************************************************************************************************/
Datum& Datum::operator=(unsigned value) {
	if (value > numeric_limits<int>::max())
		throw IllegalOp("Attempt to construct an integer with an out-of-range value");

	return *this = Datum(value);
}

/********************************************************************************************//**
 * @param	value	new value, and type
 * @return	*this, which is now an Integer
 ************************************************************************************************/
Datum& Datum::operator=(size_t value) {
	if (value > numeric_limits<int>::max())
		throw IllegalOp("Attempt to construct an integer from an out-of-range value");

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
 * @return	modified copy of this
 ************************************************************************************************/
Datum Datum::operator-() const {
	switch(kind()) {
	case Datum::Integer:	return Datum(-i);
	case Datum::Real:		return Datum(-r);
	default: throw IllegalOp("unary negation on illegal type");
	}
}

/********************************************************************************************//**
 * @return	modified copy of this
 ************************************************************************************************/
Datum Datum::operator~() const {
	if (kind() != Kind::Integer)
		throw IllegalOp("bitwise 'not' on illegal type");

	return Datum(~i);
}

/********************************************************************************************//**
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator++() {
	switch(kind()) {
	case Kind::Integer:	++i;	break;
	case Kind::Real:	++r;	break;
	default: throw IllegalOp("pre-increment on non-numeric value");
	}

	return *this;
}

/********************************************************************************************//**
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator--() {
	switch(kind()) {
	case Kind::Integer:	--i;	break;
	case Kind::Real:	--r;	break;
	default: throw IllegalOp("pre-decrement on non-numeric value");
	}

	return *this;
}

/********************************************************************************************//**
 * @return	Copy of *this before the operator
 ************************************************************************************************/
Datum Datum::operator++(int) {
	Datum prev= *this;

	switch(kind()) {
	case Kind::Integer:	++i;	break;
	case Kind::Real:	++r;	break;
	default: throw IllegalOp("pre-increment on non-numeric value");
	}

	return prev;
}

/********************************************************************************************//**
 * @return	Copy of *this before the operator
 ************************************************************************************************/
Datum Datum::operator--(int) {
	Datum prev= *this;

	switch(kind()) {
	case Kind::Integer:	--i;	break;
	case Kind::Real:	--r;	break;
	default: throw IllegalOp("pre-decrement on non-numeric value");
	}

	return prev;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator+=(const Datum& rhs) {
	switch(kind()) {
	case Kind::Integer:
		switch(rhs.kind()) {
		case Kind::Integer:	i += rhs.integer();		break;
		case Kind::Real:	i += rhs.real();		break;
		default: throw IllegalOp("Attempt to add illegal types");
		}
		break;

	case Kind::Real:
		switch(rhs.kind()) {
		case Kind::Integer:	r += rhs.integer();		break;
		case Kind::Real:	r += rhs.real();		break;
		default: throw IllegalOp("Attempt to add illegal types");
		}
		break;

	default: throw IllegalOp("Attempt to add illegal types");
	}

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator-=(const Datum& rhs) {
	switch(kind()) {
	case Kind::Integer:
		switch(rhs.kind()) {
		case Kind::Integer:	i -= rhs.integer();		break;
		case Kind::Real:	i -= rhs.real();		break;
		default: throw IllegalOp("Attempt to subtract illegal types");
		}
		break;

	case Kind::Real:
		switch(rhs.kind()) {
		case Kind::Integer:	r -= rhs.integer();		break;
		case Kind::Real:	r -= rhs.real();		break;
		default: throw IllegalOp("Attempt to subtract illegal types");
		}
		break;

	default: throw IllegalOp("Subtract on illegal types");
	}

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator*=(const Datum& rhs) {
	switch(kind()) {
	case Kind::Integer:
		switch(rhs.kind()) {
		case Kind::Integer:	i *= rhs.integer();		break;
		case Kind::Real:	i *= rhs.real();		break;
		default: throw IllegalOp("Attempt to multiply illegal types");
		}
		break;

	case Kind::Real:
		switch(rhs.kind()) {
		case Kind::Integer:	r *= rhs.integer();		break;
		case Kind::Real:	r *= rhs.real();		break;
		default: throw IllegalOp("attempt to multiply illegal types");
		}
		break;

	default: throw IllegalOp("attempt to multiply illegal types");
	}

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator/=(const Datum& rhs) {
	if (rhs.zero())
		throw DivideByZero("Attempt to divide by zero");
	
	switch(kind()) {
	case Kind::Integer:
		switch(rhs.kind()) {
		case Kind::Integer:	i /= rhs.integer(); break;
		case Kind::Real:	i /= rhs.real(); break;
		default:			throw IllegalOp("attempt to divide illegal types");
		}
		break;

	case Kind::Real:
		switch(rhs.kind()) {
		case Kind::Integer:	r /= rhs.integer();	break;
		case Kind::Real:	r /= rhs.real();	break;
		default: throw IllegalOp("attempt to divide illegal types");
		}
		break;

	default: throw IllegalOp("Attempt divide illegal types");
	}

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator%=(const Datum& rhs) {
	if (rhs.zero())
		throw DivideByZero("Attempt to divide by zero");

	switch(kind()) {
	case Kind::Integer:
		switch(rhs.kind()) {
		case Kind::Integer:	i %= rhs.integer();				break;
		default: throw IllegalOp("attempt to take remander of illegal types");
		}
		break;

	case Kind::Real:
		switch(rhs.kind()) {
		case Kind::Integer:	r = remainder(r,rhs.integer());	break;
		case Kind::Real:	r = remainder(r,rhs.real());	break;
		default: throw IllegalOp("attempt to take remander of illegal types");
		}
		break;

	default: throw IllegalOp("attempt to take remander of illegal types");
	}

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator&=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw IllegalOp("bitwise AND attempt with non-interger values");

	else if (i < 0 || rhs.integer() < 0)
		throw IllegalOp("bitwise AND attempt with negative value");

	i &= rhs.integer();

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator|=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw IllegalOp("bitwise OR attempt with non-interger values");

	else if (i < 0 || rhs.integer() < 0)
		throw IllegalOp("bitwise OR attempt with negative value");

	i |= rhs.integer();

	return *this;
}

/********************************************************************************************//**
 * @param	rhs	right-hand-side value
 * @return	*this
 ************************************************************************************************/
Datum& Datum::operator^=(const Datum& rhs) {
	if (kind() != Kind::Integer || rhs.kind() != Kind::Integer)
		throw IllegalOp("bitwise XOR attempt with non-integer values");

	else if (i < 0 || rhs.integer() < 0)
		throw IllegalOp("bitwise XOR attempt with negative value");

	i ^= rhs.integer();

	return *this;
}

/********************************************************************************************//**
 * @return return my kind
 ************************************************************************************************/
Datum::Kind Datum::kind() const				{	return k;	};

/********************************************************************************************//**
 * @return my boolean value
 ************************************************************************************************/
bool Datum::boolean() const {
	if (kind() != Datum::Boolean)
		throw IllegalOp("attempt to take boolean value of non-Boolean");

	return b;
}

/********************************************************************************************//**
 * @return my character value
 ************************************************************************************************/
char Datum::character() const {
	if (kind() != Datum::Character)
		throw IllegalOp("attempt to take character value of non-Character");

	return c;
}

/********************************************************************************************//**
 * @return my interger value
 ************************************************************************************************/
int Datum::integer() const {
	if (kind() != Datum::Integer)
		throw IllegalOp("attempt to take integer value of non-Integer");

	return i;
}

/********************************************************************************************//**
 * @invariant	Throws IllegalOp if my value is negative
 * @return my interger value
 ************************************************************************************************/
unsigned Datum::natural() const {
	if (i < 0)
		throw IllegalOp("attempt to convert negative value to unsigned");

	return i;
}

/********************************************************************************************//**
 * @note will convert an integer to a real.
 * @return my real value
 ************************************************************************************************/
double Datum::real() const {
	if (kind() == Datum::Real)
		return r;

	else if (kind() == Datum::Integer)
		return i * 1.0;

	else
		throw IllegalOp("attempt to take real value of non-Real/Integer");
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
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator<<(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.integer() << rhs.integer());
}

/********************************************************************************************//**
 * @invariant	undefined if lhs.kind() != rhs.kind() != Datum::Integer
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs << rhs
 ************************************************************************************************/
Datum operator>>(const Datum& lhs, const Datum& rhs) {
	if (lhs.kind() != rhs.kind())
		assert(false);
	return Datum(lhs.integer() >> rhs.integer());
}

/********************************************************************************************//**
 * @note	use std::rel_ops for <=, !=, >, >=
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs < rhs
 ************************************************************************************************/
bool operator<(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:
		switch(rhs.kind()) {
			case Datum::Boolean:	return lhs.boolean() < rhs.boolean();
			default:				assert(false);
		}
		break;

	case Datum::Character:
		switch(rhs.kind()) {
			case Datum::Character:	return lhs.character() < rhs.character();
			case Datum::Integer:	return lhs.character() < rhs.integer();
			default:				assert(false);
		}
		break;

	case Datum::Integer:
		switch(rhs.kind()) {
			case Datum::Character:	return lhs.integer() < rhs.character();
			case Datum::Integer:	return lhs.integer() < rhs.integer();
			case Datum::Real:		return lhs.integer() < rhs.real();
			default:				assert(false);
		}
		break;

	case Datum::Real:
		switch(rhs.kind()) {
			case Datum::Character:	return lhs.real()	< rhs.character();
			case Datum::Integer:	return lhs.real()  < rhs.integer();
			case Datum::Real:		return lhs.real()  < rhs.real();
			default:				assert(false);
		}
		break;

	default:			assert(false);
	}

	return false;
}

/********************************************************************************************//**
 * @note	use std::rel_ops for <=, !=, >, >=
 * @param	lhs	The left-hand-side
 * @param	rhs The right-hand-side
 *
 * @return lhs == rhs
 ************************************************************************************************/
bool operator==(const Datum& lhs, const Datum& rhs) {
	switch(lhs.kind()) {
	case Datum::Boolean:
		switch(rhs.kind()) {
			case Datum::Boolean:	return lhs.boolean() == rhs.boolean();
			default:				assert(false);
		}
		break;

	case Datum::Integer:
		switch(rhs.kind()) {
			case Datum::Integer:	return lhs.integer() == rhs.integer();
			case Datum::Real:		return lhs.integer() == rhs.real();
			default:				assert(false);
		}
		break;

	case Datum::Real:
		switch(rhs.kind()) {
			case Datum::Integer:	return lhs.real()  == rhs.integer();
			case Datum::Real:		return lhs.real()  == rhs.real();
			default:				assert(false);
		}
		break;

	default:			assert(false);
	}

	return false;
}

/********************************************************************************************//**
 * @invariant	undefined if parameters are Boolean
 * @return lhs && rhs
 ************************************************************************************************/
bool operator&&(const Datum& lhs, const Datum& rhs) {
	return lhs.boolean()	&& rhs.boolean();
}

/********************************************************************************************//**
 * @invariant	undefined if parameters are Boolean
 * @return lhs || rhs
 ************************************************************************************************/
bool operator||(const Datum& lhs, const Datum& rhs) {
	return lhs.boolean()	|| rhs.boolean();
}

