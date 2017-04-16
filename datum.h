/** @file datum.h
 *
 *  PL0/C Machine's data type
 *
 *  A Datum maybe an Interger, for values, Unsigned, for data addresses, or Real values.
 */

#ifndef	DATUM_H
#define DATUM_H

#include <cstdint>
#include <vector>

namespace pl0c {
	/// A signed integer value or memory offset
	typedef std::int32_t					Integer;

	/// An unsigned integer value or memory address
	typedef std::uint32_t					Unsigned;

	/// A floating point value
	typedef double							Real;

	/// PL0C data types
	enum class Type {
		Integer,							///< Signed integer
		Real								///< Floating point number
	};

	/** A PL0C Data Value
	 *
	 *  A signed, or unsigned, integer, or a floating point (real) value.
	 *
	 *  @note No discrimunator is necessary as the union only contains simple types, and there is no
	 *  	  requirement for runtime type promotion or demotion.
	 */
	union Datum {
		pl0c::Integer	i;							///< As a signed integer
		pl0c::Unsigned	u;							///< As a unsigned integer
		pl0c::Real		r;							///< As a real

		/// Construct from a signed integer, defaults to zero
		Datum(pl0c::Integer value = 0);
		Datum(pl0c::Unsigned value);				///< Construct an unsigned integer
		Datum(std::size_t value);					///< Construct an unsigned (size_t)
		Datum(pl0c::Real value);					///< Constuct from a double
	};

	/// A vector of Datums
	typedef	std::vector<Datum>	DatumVector;
}

#endif
