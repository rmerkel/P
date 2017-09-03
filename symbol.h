/********************************************************************************************//**
 * @file	symbol.h
 *
 * The Pascal-Lite Symbol table
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <cstdint>
#include <map>
#include <sstream>

#include "datum.h"
#include "type.h"

/********************************************************************************************//**
 * A Symbol table entry
 *
 * Describes a varaible, constant value, procedure or function or an entry in the type system.
 * Every entry has a block/activation frame level, setting the objects scope in the program. In
 * addition:
 * - A Varaible is a location, as an offset from the block/frame, n levels down, and a type.
 * - A Constant is just a Datum value and its type
 * - Subroutines, Functions and Procedures, have an absolute entry point address and a vector of
 *   formal parameter types. In addition, Funcitons have a return type while Procedures are 
 *   untyped.
 * - A Type is a type descriptor in the type system.
 ************************************************************************************************/
class SymValue {
public:
	/// Kinds of symbol table entries
	enum class Kind : char {
		None,									///< Placeholder for a valid kind...
		Variable,		  						///< A variable location and type
		Constant,								///< A constant value and type
		Procedure,								///< A procedure entry point
		Function,								///< A function entry point and return type
		Type									///< Entry in the type system
	};

	static std::string toString(Kind kind);		///< Return a kind as a string

	typedef std::vector<Kind> KindVec;			///< Vector of Kinds
	
	SymValue();									///< Default constructor; undefined entry

	/// Construct a constant value
	SymValue(int level, Datum value, TDescPtr type);

	/// Construct a Variable location
	SymValue(int level, Integer offset, TDescPtr type);

	SymValue(Kind kind, int level);				///< Partially construct Procedure or Function

	SymValue(int level, TDescPtr type);			///< Construct Type

	/// Descructor
	virtual ~SymValue()							{}

	Kind kind() const;							///< Return my kind

	int level() const;							///< Return my activation frame level

	Datum value(Datum value);					///< Set, and return, my value
	Datum value() const;						///< Return my value

	TDescPtrVec& params();						///< Return my subrountine parameter kinds
	const TDescPtrVec& params() const;			///< Return my subrountine parameter kinds

	ConstTDescPtr type(TDescPtr type);			///< Set and return my type
	ConstTDescPtr type() const;					///< Return my type


private:
	Kind			_kind;						///< None, Variable, Procedure, Function or Type
	int				_level;						///< Block level (scope) for all types
	Datum			_value;						///< Variable frame offset, Constant value or subroutine address
	TDescPtr		_type;						///< Type, n/a for Procedures
	TDescPtrVec		_params;					///< Subroutine parameter kinds
};

/********************************************************************************************//**
 * A SymbolTable; a multimap of symbol identifiers to SymValue's
 ************************************************************************************************/
typedef std::multimap<std::string, SymValue> SymbolTable;

#endif
