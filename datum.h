/** @file datum.h
 *
 *  PL0/C Machine's data type
 *
 *  A Datum maybe an Interger, for values, Unsigned, for data addresses, or Real values.
 */

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <vector>

/** A PL0C Data Value
 *  
 *  Datums may contain a signed or unsigned integer, or a floating point/real value. Signed
 *  and unsigned integer values are freely interchangeable, but conversion to from a real is
 *  undefined.
 *  
 *  A discriminator (k), which is initialized by the constructors, is provided, but not
 *  enforced.
 */
struct Datum {
	typedef int			Integer;			///< Signed integer value or memory offset
	typedef unsigned	Unsigned;			///< Unsigned integer value or address
	typedef double		Real;				///< Floating-point value	

	/// Datum "kinds"
	enum class Kind {
		Integer,							///< Integer
		Unsigned,							///< Unsigned
		Real								///< Real
	};

	typedef std::vector<Kind>	KindVec;	///< Vector of Kind's

	static std::string toString(Kind k);	///< Return k as a string...

	Datum();								///< Default constructor...
	Datum(Integer value);					///< Construct a signed integer...
	Datum(Unsigned value);					///< Construct an unsigned integer...
	Datum(std::size_t value);				///< Construct an unsigned (size_t)...
	Datum(Real value);						///< Constuct from a double...

	union {
		Integer		i;						///< k == Integer
		Unsigned	u;						///< k == Unsigned
		Real		r;						///< k == Real
	};
	Kind			k;  					///< What Datum type?
};

/// Datum put operator
std::ostream& operator<<(std::ostream& os, const Datum& d);

typedef	std::vector<Datum>	DatumVector;	///< A vector of Datums

#endif
