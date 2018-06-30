/********************************************************************************************//**
 * @file	symbol.h
 *
 * The P Compilier Symbol table
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <map>
#include <sstream>

#include "datum.h"
#include "type.h"

/********************************************************************************************//**
 * A Symbol Table Entry
 *
 * Describes a single varaible, constant value, procedure, function, or an entry in the type
 * system. Each has a block/activation frame level, setting the objects scope in the program. In
 * addition:
 * - A Varaible is a location, as an offset from the block/frame, n levels down, and a type.
 * - A Constant is just a Datum value and its type
 * - Subroutines, Functions and Procedures, have an absolute entry point address and a vector of
 *   formal parameter types. Funcitons have a return type while Procedures are untyped.
 * - A Type is a type descriptor in the type system.
 ************************************************************************************************/
class SymValue {
public:
	/// Kinds of symbol table entries
	enum Kind {
		None,									///< Placeholder for a valid kind...
		Variable,		  						///< A variable location and type
		Constant,								///< A constant value and type
		Procedure,								///< A procedure entry point
		Function,								///< A function entry point and return type
		Type									///< Entry in the type system
	};

	typedef std::vector<Kind> KindVec;			///< Vector of Kinds
	
	static SymValue makeConst(int level, const Datum& value, TDescPtr type);
	static SymValue makeVar(int level, int ofset, TDescPtr type);
	static SymValue makeSbr(Kind kind, int level);
	static SymValue makeType(int level, TDescPtr);

	SymValue();									///< Default constructor; undefined entry
	SymValue(Kind kind, int level, const Datum& value, TDescPtr type, const TDescPtrVec& params);

	virtual ~SymValue()	{}						///< Destructor

	Kind kind() const;							///< Return my kind
	int level() const;							///< Return my activation frame level

	bool returned(bool r);						///< Set, and return, my returned value
	bool returned() const;						///< Return my returned value

	Datum value(Datum value);					///< Set, and return, my value
	Datum value() const;						///< Return my value

	TDescPtrVec& params();						///< Return my subrountine parameter kinds
	const TDescPtrVec& params() const;			///< Return my subrountine parameter kinds

	TDescPtr type(TDescPtr type);				///< Set and return my type
	TDescPtr type() const;						///< Return my type

private:
	Kind			_kind;						///< None, Variable, Procedure, Function or Type
	bool			_returned;					///< Did the function return a value?
	int				_level;						///< Block level (scope) for all types
	Datum			_value;						///< Variable frame offset, Constant value or Subroutine address
	TDescPtr		_type;						///< Type, n/a for Procedures
	TDescPtrVec		_params;					///< Subroutine parameter kinds
};

std::ostream& operator<<(std::ostream& os, const SymValue::Kind& kind);

/********************************************************************************************//**
 * A SymbolTable; a multimap of symbol identifiers to SymValue's
 ************************************************************************************************/
typedef std::multimap<std::string, SymValue> SymbolTable;

/********************************************************************************************//**
 * A SymbolTable iterator
 ************************************************************************************************/
typedef SymbolTable::iterator	SymbolTableIter;

/********************************************************************************************//**
 * A SymbolTable entry
 ************************************************************************************************/
typedef SymbolTable::value_type	SymbolTableEntry;

#endif
