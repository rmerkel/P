/**	@file	symbol.h
 *
 *	@brief	the Symbol table
 *
 *	Created by Randy Merkel on 6/7/2013.
 *  Copyright (c) 2016 Randy Merkel. All rights reserved.
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <cstdint>
#include <map>
#include <sstream>

#include "pl0c.h"

/// A Symbol table entry
struct SymValue {
	/// Kinds of symbol table entries
	enum Kind : char {
		none,									///< Placeholder for a 'real' token
		identifier,		  						///< An identifier
		constant,								///< A constant identifier
		proc,									///< A procedure
	};

	static std::string toString(Kind k);		///< Return k as a string

	Kind		kind;							///< identifier, constant, or procedure name
	unsigned	level;							///< If kind == proc
	pl0c::Word	value;							///< value (identifier, constant) or address of a proc)

	/// Default construction
	SymValue() : kind {none}, level {0}, value{0}
		{}

	/// Constructor; create a SymValue from it's components
	SymValue(Kind k, unsigned l = 0, pl0c::Word v = 0) : kind{k}, level{l}, value{v}
		{}
};

/// A SymbolTable
typedef std::multimap<std::string, SymValue> SymbolTable;

#endif
