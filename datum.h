/** @file datum.h
 *
 *  PL0/C Machine's data type
 *
 *  A Datum maybe an Interger, for values, Unsigned, for data addresses, or Real values.
 */

#ifndef	DATUM_H
#define DATUM_H

#include <vector>

namespace pl0c {
		/// A signed integer value or memory offset
	typedef int32_t							Integer;

	/// An unsigned integer value or memory address
	typedef uint32_t						Unsigned;

	/// A floating point value
	typedef double							Real;

	/// A single signed or unsigned integer, or a float (real)
	struct Datum {
		/// Data kinds
		enum class Kind {
			Integer, 								///< A signed integer
			Unsigned, 								///< A unsigned integer
			Real									///< A floating point/real
		};

		union {
			int 		i;							///< Value if type == Integer
			unsigned	u;							///< Value if type == Unsigned
			double      r;							///< Value if type == Real
		  };
		  Kind 			kind;						///< What kind of datum?

		/// Construct from an integer, defaults to zero
		Datum(int value = 0)	: kind{Kind::Integer} 	{	i = value;	}

		/// Construct from an unsigned integer
		Datum(unsigned value)	: kind{Kind::Unsigned} 	{	u = value;	}

		/// Constuct from a double
		Datum(double value) 	: kind{Kind::Real} 	{	r = value;	}
	};

	/// A vector of Datums
	typedef	std::vector<Datum>	DatumVector;
}

#endif
