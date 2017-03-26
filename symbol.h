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
#include "pl0c.h"

/// A Symbol table entry
struct SymValue {
	/// Kinds of symbol table entries
	enum Kind : char {
		none,									///< Placeholder for a 'real' token
		identifier,		  						///< An identifier
		constant,								///< A constant identifier
		proc,									///< A procedure
		function								///< A function
	};

	static std::string toString(Kind k);		///< Return k as a string

	Kind			kind;						///< identifier, constant, funciton, or procedure name
	int				level;						///< Base/frame level If kind == proc or function
	pl0c::Datum		value;						///< value (identifier, constant) or address of a proc or function

	/// Default construction
	SymValue() : kind {none}, level {0}, value{0}
		{}

	/** Construct a SymValue from it's components...
	 *
	 * @param _kind		The token kind, e.g., identifier
	 * @param _level	The token base/frame level, e.g., 0 for "current frame.
	 * @param _value 	The token value, e.g., a procedure address
	 */
	SymValue(Kind _kind, int _level = 0, pl0c::Integer _value = 0) : kind{_kind}, level{_level}, value{_value}
		{}
};

/// A SymbolTable; a multimap of symbol identifiers to SymValue's
typedef std::multimap<std::string, SymValue> SymbolTable;

#endif
