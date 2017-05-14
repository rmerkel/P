/** @file datum.h
 *
 * PL0/C Machine's data types(s)
 *
 * A given Datum may be a signed or unsigned integer, or real (floating point).
 * Uses std::rel_ops to implement Datum comparison operators.
 */

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <utility>
#include <vector>

using namespace std::rel_ops;

/** A PL0C Data Value
 *  
 * Once Datum's are constructed, or assigned a value, they retain that value, 
 * but can be freely converted to other types on demand. Conversations result in 
 * copies of the unchanged orignal. 
 *  
 * Binary operations of mixed type, automatically promote returning a new Datum 
 * of the new type. 
 */
class Datum {
public:
	typedef int			Integer;			///< Signed integer value or memory offset
	typedef unsigned	Unsigned;			///< Unsigned integer value or address
	typedef double		Real;				///< Floating-point value	

	/// Datum "kinds"
	enum class Kind {
		Integer,							///< Signed integer
		Unsigned,							///< Unsigned integer
		Real								///< Real (floating point)
	};

	static std::string toString(Kind k);	///< Return k's name, e.g., "real" for Real...

	Datum();								///< Default constructor...
	explicit Datum(Integer value);			///< Construct a signed integer...
	explicit Datum(Unsigned value);			///< Construct an unsigned integer...
	explicit Datum(std::size_t value);		///< Construct an unsigned (size_t)...
	explicit Datum(Real value);				///< Constuct from a double...

	/// @return my kind...
	Kind kind() const						{	return k;   }

	Integer integer() const;				///< Return Datum as an integer
	Unsigned uinteger() const;				///< Return Datum as an unsigned
	Real real() const;						///< Return Datum as an real

	Datum& operator=(Integer value);		///< Assign a new signed integer value
	Datum& operator=(Unsigned value);		///< Assign a new unsigned integer value
	Datum& operator=(std::size_t value);	///< Assign a new unsigned integer value
	Datum& operator=(Real value);			///< Assign a new real value

private:
	union {
		Integer		i;						///< k == Integer
		Unsigned	u;						///< k == Unsigned
		Real		r;						///< k == Real
	};
	Kind			k;  					///< What Datum type?
};

// unary operators

Datum operator!(const Datum& rhs);			///< Return the not of a Datum
Datum operator-(const Datum& rhs);			///< Return the negative of a Datum
Datum operator~(const Datum& rhs);			///< Return the 1s complement of a Datum

// binary operators

/// Return the sum of two Datums
Datum operator+(const Datum& lhs, const Datum& rhs);

/// Return the difference of two Datums
Datum operator-(const Datum& lhs, const Datum& rhs);

/// Return the product of two Datums
Datum operator*(const Datum& lhs, const Datum& rhs);

/// Return the quotient of two Datums
Datum operator/(const Datum& lhs, const Datum& rhs);

/// Return the quotient of two Datums
Datum operator%(const Datum& lhs, const Datum& rhs);

/// Return bit-wise or OR two Datums
Datum operator|(const Datum& lhs, const Datum& rhs);

/// Return bit-wise AND of two Datums
Datum operator&(const Datum& lhs, const Datum& rhs);

/// Return bit-wise XOR of two Datums
Datum operator^(const Datum& lhs, const Datum& rhs);

/// Return lhs left shifted by rhs
Datum operator<<(const Datum& lhs, const Datum& rhs);

/// Return lhs right shifted by rhs
Datum operator>>(const Datum& lhs, const Datum& rhs);

// logical operators

/// Return true if lhs is less than rhs
bool operator<(const Datum& lhs, const Datum& rhs);

/// Return true if lhs is equal to rhs
bool operator==(const Datum& lhs, const Datum& rhs);

/// Return true if lhs or rhs is true
bool operator||(const Datum& lhs, const Datum& rhs);

/// Return true if lhs and rhs are true
bool operator&&(const Datum& lhs, const Datum& rhs);

// stream operators

/// Datum put operator
std::ostream& operator<<(std::ostream& os, const Datum& d);

typedef	std::vector<Datum>	DatumVector;	///< A vector of Datums

#endif
