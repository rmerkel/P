/********************************************************************************************//**
 * @file datum.h
 *
 *  Pascal-Lite Machine's data type
 *
 * A Datum maybe an Interger, for values, Unsigned, for data addresses, or
 * Real values. Includes vectors or Ranges of Datum's. 
 ************************************************************************************************/

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <vector>
#include <utility>

#include "types.h"

/********************************************************************************************//**
 * A Pascal-Lite Data Value
 *  
 * Datums contain a signed or unsigned integer, or a floating point/real value.
 * Signed and unsigned integer values are freely interchangeable, but
 * conversion from a real, say calling real() on a Integer, is undefined.
 *  
 * A discriminator (kind()), which is initialized by the constructors, is
 * provided, but only partially enforced by some operators, via assert(), i.e.,
 * bitwise operators on Real values is undefined.
 ************************************************************************************************/
class Datum {
public:
	/// Datum "kinds"
	enum class Kind {
		Integer,							///< Signed integer
		Unsigned,							///< Unsigned integer
		Real								///< Real
	};

	typedef std::vector<Kind>	KindVec;	///< Vector of Datum Kind's

	static std::string toString(Kind k);	///< Return k as a string...

	Datum();								///< Default constructor...
	Datum(bool value);						///< Construct a boolean, as a integer
	Datum(Integer value);					///< Construct a signed integer...
	Datum(Unsigned value);					///< Construct an unsigned integer...
	Datum(std::size_t value);				///< Construct an unsigned (size_t)...
	Datum(Real value);						///< Constuct from a double...
	virtual ~Datum() {}						///< Destructor

	Datum operator!() const;				///< Unary boolean negation...
	Datum operator-() const;				///< Unary negation...
	Datum operator~() const;				///< Unary bitwise not...

	/// Return my kind...
	Kind kind() const						{	return k;	};

	/// Return my integer value...
	Integer integer() const					{	return i;	};

	/// Return my unsigned value...
	Unsigned uinteger() const				{	return u;	};

	/// Retrun my real value...
	Real real() const						{	return r;	};

private:
	union {
		Integer		i;						///< k == Integer
		Unsigned	u;						///< k == Unsigned
		Real		r;						///< k == Real
	};
	Kind			k;  					///< What Datum type?
};

/********************************************************************************************//**
 * A vector of Datums
 ************************************************************************************************/
typedef	std::vector<Datum>	DatumVector;

std::ostream& operator<<(std::ostream& os, const Datum& d);

Datum operator+	(const Datum& lhs,	const Datum& rhs);
Datum operator-	(const Datum& lhs,	const Datum& rhs);
Datum operator*	(const Datum& lhs,	const Datum& rhs);
Datum operator/	(const Datum& lhs,	const Datum& rhs);
Datum operator%	(const Datum& lhs,	const Datum& rhs);

Datum operator&	(const Datum& lhs,	const Datum& rhs);
Datum operator|	(const Datum& lhs,	const Datum& rhs);
Datum operator^	(const Datum& lhs,	const Datum& rhs);
Datum operator<<(const Datum& lhs,	const Datum& rhs);
Datum operator>>(const Datum& lhs,	const Datum& rhs);

bool operator<	(const Datum& lhs,	const Datum& rhs);
bool operator<=(const Datum& lhs,	const Datum& rhs);
bool operator==(const Datum& lhs,	const Datum& rhs);
bool operator>=(const Datum& lhs,	const Datum& rhs);
bool operator>	(const Datum& lhs,	const Datum& rhs);
bool operator!=(const Datum& lhs,	const Datum& rhs);

bool operator&&(const Datum& lsh, const Datum& rhs);
bool operator||(const Datum& lsh, const Datum& rhs);

#endif

