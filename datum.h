/********************************************************************************************//**
 * @file datum.h
 *
 * The P Machine's data type
 *
 * class Datum
 ************************************************************************************************/

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <vector>
#include <utility>

/********************************************************************************************//**
 * A Data Value
 *  
 * Datums contain either a signed or unsigned interger, a boolean, character, or a floating
 * point/real value. Signed and unsigned integer values are freely interchangeable, but
 * conversion from a real, say calling real() on a integer, is undefined.
 *  
 * A discriminator (kind()), which is initialized by the constructors, is provided, but only
 * partially enforced by some operators, via assert(), i.e., bitwise operators on Real values
 * are undefined.
 ************************************************************************************************/
class Datum {
public:
	/// Datum "kinds"
	enum Kind {
		Boolean,							///< Boolean value
		Character,							///< Character (ASCII) value
		Integer,							///< Signed integer
		Real								///< Floating point
	};

	Datum();								///< Default constructor...
	Datum(bool value);						///< Construct a boolean...
	Datum(char value);						///< Construct a character...
	Datum(int value);						///< Construct a signed integer...
	Datum(unsigned value);					///< Construct an unsigned integer...
	Datum(std::size_t value);				///< Construct an unsigned (size_t)...
	Datum(double value);					///< Constuct from a double...
	virtual ~Datum() {}						///< Destructor

	Datum operator!() const;				///< Unary boolean negation...
	Datum operator-() const;				///< Unary negation...
	Datum operator~() const;				///< Unary bitwise not...

	Kind kind() const;						///< Return my kind...

	bool boolean() const;					///< Return my boolean value...
	char character() const;					///< Return my character...
	int integer() const;					///< Return my integer value...
	unsigned natural() const;				///< Return my natual (unsigned) value...
	double real() const;					///< Return my real value...

	bool numeric() const;					///< Return true if value is numeric...
	bool ordinal() const;					///< Return true if value is ordinal...

private:
	union {
		bool		b;						///< k == Boolean
		char		c;						///< k == Character
		int			i;						///< k == Integer
		unsigned	u;						///< k == Integer, but need unsigned
		double		r;						///< k == Real
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
bool operator<=(const Datum& lhs, const Datum& rhs);
bool operator==(const Datum& lhs, const Datum& rhs);
bool operator>=(const Datum& lhs, const Datum& rhs);
bool operator> (const Datum& lhs, const Datum& rhs);
bool operator!=(const Datum& lhs, const Datum& rhs);

bool operator&&(const Datum& lsh, const Datum& rhs);
bool operator||(const Datum& lsh, const Datum& rhs);

#endif

