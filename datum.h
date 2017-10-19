/********************************************************************************************//**
 * @file datum.h
 *
 *  Pascal-lite Machine's data type
 *
 * A Datum maybe an Interger, for values or for data addresses, or Real values. Includes vectors
 * and Ranges of Datum's. 
 ************************************************************************************************/

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <vector>
#include <utility>

/********************************************************************************************//**
 * A Data Value
 *  
 * Datums contain a signed or unsigned integer, or a floating point/real value.
 * Signed and unsigned integer values are freely interchangeable, but
 * conversion from a real, say calling real() on a integer, is undefined.
 *  
 * A discriminator (kind()), which is initialized by the constructors, is
 * provided, but only partially enforced by some operators, via assert(), i.e.,
 * bitwise operators on Real values is undefined.
 ************************************************************************************************/
class Datum {
public:
	/// Datum "kinds"
	enum Kind {
		Integer,							///< Signed integer
		Real								///< Floating point
	};

	Datum();								///< Default constructor...
	Datum(bool value);						///< Construct a boolean, as a integer
	Datum(int value);						///< Construct a signed integer...
	Datum(unsigned value);					///< Construct an unsigned integer...
	Datum(std::size_t value);				///< Construct an unsigned (size_t)...
	Datum(double value);					///< Constuct from a double...
	virtual ~Datum() {}						///< Destructor

	Datum operator!() const;				///< Unary boolean negation...
	Datum operator-() const;				///< Unary negation...
	Datum operator~() const;				///< Unary bitwise not...

	Kind kind() const;						///< Return my kind...

	int integer() const;					///< Return my integer value...
	unsigned natural() const;				///< Return my natual (unsigned) value...
	double real() const;					///< Return my real value...

private:
	union {
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

bool operator<(const Datum& lhs, const Datum& rhs);
bool operator<=(const Datum& lhs, const Datum& rhs);
bool operator==(const Datum& lhs, const Datum& rhs);
bool operator>=(const Datum& lhs, const Datum& rhs);
bool operator> (const Datum& lhs, const Datum& rhs);
bool operator!=(const Datum& lhs, const Datum& rhs);

bool operator&&(const Datum& lsh, const Datum& rhs);
bool operator||(const Datum& lsh, const Datum& rhs);

#endif

