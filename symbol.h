/**	@file	symbol.h
 *
 * The P/L0C Symbol table
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <cstdint>
#include <map>
#include <sstream>

#include "datum.h"

/** A Symbol table entry
 *
 *  Describes a variable location (activation frame offset), a constant value or a subroutine
 *  (procedure or function) entry point and parameter count.
 */
class SymValue {
public:
	/// Kinds of symbol table entries
	enum Kind : char {
		None,									///< Placeholder for a valid kind...
		Variable,		  						///< An identifier
		ConstInt,								///< A constant integer value
		ConstReal,								///< A constant floating point value
		Procedure,								///< A procedure
		Function								///< A function
	};

	static std::string toString(Kind k);		///< Return k as a string

	SymValue();									///< Default constructor

	/// Construct a SymValue from it's components...
	SymValue(Kind kind, int level = 0, pl0c::Datum value = pl0c::Datum{}, std::size_t nArgs = 0);

	/// Descructor
	virtual ~SymValue()							{}

	/// Return my Kind...
	Kind kind() const;							///< Return my kind
	int level() const;							///< Return my activation frame level

	pl0c::Datum value(pl0c::Datum value);		///< Set my value
	pl0c::Datum value() const;					///< Return my value

	std::size_t nArgs(std::size_t value);		///< Set my formal parameter count
	std::size_t nArgs() const;					///< Return my formal parameter count

private:
	Kind			k;							///< Identifier, ConstInt...
	int				l;							///< Activation frame level for Variables and subroutines.
	pl0c::Datum		v;							///< Variable frame offset, Constant value or subroutine address
	std::size_t		n;							///< Subrountine formal argument count

};

/// A SymbolTable; a multimap of symbol identifiers to SymValue's
typedef std::multimap<std::string, SymValue> SymbolTable;

#endif
