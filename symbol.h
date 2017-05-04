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
 * Each entry describes one of the following objects:
 * - Constant Datum value, type and block level, setting it's scope.
 * - Variable location, as offset from a block/frame, n levels down, and its Datum type.
 * - Procedure entry point, it's activation block/frame level, and number of formal parameters. 
 * - Same as procedure, but with the additon of a return Datum type
 */
class SymValue {
public:
	/// Kinds of symbol table entries
	enum class Kind : char {
		None,									///< Placeholder for a valid kind...
		Variable,		  						///< A variable location
		Constant,								///< A constant value
		Procedure,								///< A procedure entry point
		Function,								///< A function entry point and return value
	};

	static std::string toString(Kind k);		///< Return a kind as a string

	SymValue();									///< Default constructor; undefined entry
	SymValue(int level, Datum value);			///< Construct a constant value

	/// Construct a Variable location
	SymValue(int level, Datum::Integer value, Datum::Kind type);

	SymValue(Kind kind, int level);				///< Construct procedure or funciton

	/// Descructor
	virtual ~SymValue()							{}

	Kind kind() const;							///< Return my kind
	int level() const;							///< Return my activation frame level
	Datum value(Datum value);					///< Set my value
	Datum value() const;						///< Return my value
	Datum::Kind type(Datum::Kind value);		///< Set my function return type
	Datum::Kind type() const;					///< Return my function return type
	std::size_t nArgs(std::size_t value);		///< Set my formal parameter count
	std::size_t nArgs() const;					///< Return my formal parameter count

private:
	Kind		k;								///< None, Variable, Procedure or Function
	int			l;								///< Activation frame level for Variables and subroutines.
	Datum		v;								///< Variable frame offset, Constant value or subroutine address
	Datum::Kind	t;								///< Datum value type	
	std::size_t	n;								///< Subrountine formal argument count
};

/// A SymbolTable; a multimap of symbol identifiers to SymValue's
typedef std::multimap<std::string, SymValue> SymbolTable;

#endif
