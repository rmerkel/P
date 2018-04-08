/********************************************************************************************//**
 * @file datum.h
 *
 * The P Machine's data type.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <utility>
#include <vector>

/********************************************************************************************//**
 * A Data Value
 *  
 * Datums contain either a boolean, character, signed interger, floating point value.
 * Convertsion between types is limited to signed and unsigned intergers, as long as signed
 * values are limited from 0..std::numeric_limits<int>::max().
 *  
 * A discriminator (kind()), which is initialized by the constructors, and enforced by the
 * operators, which throw a Datum::Error, i.e., bitwise operators on Real values are illegal.
 ************************************************************************************************/
class Datum {
public:
	/// Datum "kinds"
	enum Kind {
		Boolean,							///< Boolean value
		Character,							///< Character (ASCII) value
		Integer,							///< Signed integer
		Real,								///< Floating point
	};

	Datum();								///< Default constructor...
	Datum(const Datum& datum);				///< Copy constructor
	Datum(Datum&& datum);					///< Move constructor

	explicit Datum(bool value);				///< Construct a Boolean...
	explicit Datum(char value);				///< Construct a Character...
	explicit Datum(int value);				///< Construct a Integer...
	explicit Datum(unsigned value);			///< Construct a Integer...
	explicit Datum(size_t value);			///< Construct a Integer...
	explicit Datum(double value);			///< Construct a Double...
	virtual ~Datum() {}						///< Destructor

	Datum& operator=(const Datum& value);	///< Assignment...
	Datum& operator=(Datum&& value);		///< Move assignment...

	Datum& operator=(bool value);			///< Assignment (Boolean)...
	Datum& operator=(char value);			///< Assignment (Character)...
	Datum& operator=(int value);			///< Assignment (Integer)...
	Datum& operator=(unsigned value);		///< Assignment (Integer)...
	Datum& operator=(size_t value);			///< Assignment (Integer)...
	Datum& operator=(double value);			///< Assignment (Real)...

	Datum operator!() const;				///< Unary boolean negation...
	Datum operator-() const;				///< Unary negation...
	Datum operator~() const;				///< Unary bitwise not...

	Datum& operator++();					///< Prefix increment
	Datum& operator--();					///< Prefix decrement
	Datum operator++(int);					///< Postfix increment
	Datum operator--(int);					///< Postfix decrement

	Datum& operator+=(const Datum& rhs);	///< Subtract...
	Datum& operator-=(const Datum& rhs);	///< Subtract...
	Datum& operator*=(const Datum& rhs);	///< Multiplication...
	Datum& operator/=(const Datum& rhs);	///< Division...
	Datum& operator%=(const Datum& rhs);	///< Remander...

	Datum& operator&=(const Datum& rhs);	///< Bitwise And...
	Datum& operator|=(const Datum& rhs);	///< Bitwise Or...
	Datum& operator^=(const Datum& rhs);	///< Bitwise Exclusive-Or...

	Datum& operator>>=(const Datum& rhs);	///< Bitwise shift-right...
	Datum& operator<<=(const Datum& rhs);	///< Bitwise shift-left...

	Kind kind() const;						///< Return my kind...

	bool boolean() const;					///< Return my Boolean value...
	char character() const;					///< Return my Character...
	int integer() const;					///< Return my Integer value...
	unsigned natural() const;				///< Return my Integer value, but as an unsigned
	double real() const;					///< Return my Real value...

	bool numeric() const;					///< Return true if value is numeric...
	bool ordinal() const;					///< Return true if value is ordinal...
	bool zero() const;						///< Return true if value is equal to zero...

private:
	union {
		bool		b;						///< if k == Boolean
		char		c;						///< if k == Character
		int			i;						///< if k == Integer
		double		r;						///< if k == Real
	};
	Kind			k;  					///< What Datum type?
};

/********************************************************************************************//**
 * A vector of Datums
 ************************************************************************************************/
typedef	std::vector<Datum>	DatumVector;

std::ostream& operator<<(std::ostream& os, const Datum::Kind& kind);
std::ostream& operator<<(std::ostream& os, const Datum& value);

Datum operator+(const Datum& lhs, const Datum& rhs);
Datum operator-(const Datum& lhs, const Datum& rhs);
Datum operator*(const Datum& lhs, const Datum& rhs);
Datum operator/(const Datum& lhs, const Datum& rhs);
Datum operator%(const Datum& lhs, const Datum& rhs);

Datum operator&(const Datum& lhs, const Datum& rhs);
Datum operator|(const Datum& lhs, const Datum& rhs);
Datum operator^(const Datum& lhs, const Datum& rhs);

Datum operator<<(const Datum& lhs, const Datum& rhs);
Datum operator>>(const Datum& lhs, const Datum& rhs);

bool operator <(const Datum& lhs, const Datum& rhs);
bool operator==(const Datum& lhs, const Datum& rhs);

bool operator&&(const Datum& lsh, const Datum& rhs);
bool operator||(const Datum& lsh, const Datum& rhs);

#endif

