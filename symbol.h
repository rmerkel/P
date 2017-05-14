/**	@file	symbol.h
 *  
 * Symbol table definitions for the PL0C compilier.
 *  
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <map>
#include <sstream>

#include "datum.h"

/** A Symbol table entry
 *
 * Each entry describes one of the following objects:
 * - Constant Datum value and block level, setting it's scope.
 * - Variable location, as offset from a block/frame, n levels down.
 * - Subroutine entry point, it's activation block/frame level, and count of
 *   formal parameters.
 */
class SymValue {
public:
	/// Kinds of symbol table entries
	enum class Kind : char {
		None,								///< Placeholder for a valid kind...
		Variable,		  					///< A variable location
		Constant,							///< A constant value
		Procedure,							///< A procedure entry point
		Function,							///< A function entry point and return value
	};

	static std::string toString(Kind k);	///< Return kind as a string, e.g, "none" for None.
	
	SymValue();								///< Default constructor; undefined entry

	/// Construct a constant value...
	explicit SymValue(int level, Datum value);

	/// Construct a Variable location...
	explicit SymValue(int level, Datum::Integer value);

	explicit SymValue(Kind kind, int level); ///< Construct procedure or funciton

	/// Descructor
	virtual ~SymValue()						{}

	Kind kind() const;						///< Return my kind
	int level() const;						///< Return my activation frame level
	Datum value(Datum value);				///< Set and return my value
	Datum value() const;					///< Return my value
	std::size_t nArgs() const;				///< Return number of subroutine parameters
	std::size_t nArgs(std::size_t n);		///< Set and return number of subroutine parameters

private:
	Kind			k;						///< None, Variable, Procedure or Function
	int				l;						///< Activation frame level for Variables and subroutines.
	Datum			v;						///< Variable frame offset, Constant value or subroutine address
	std::size_t		n;						///< Subroutine parameter count
};

/// A SymbolTable; a multimap of symbol identifiers to SymValue's
typedef std::multimap<std::string, SymValue> SymbolTable;

#endif

