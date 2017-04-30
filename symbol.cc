/**	@file	symbol.cc
 *
 * The PL/0C Symbol table
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "symbol.h"

#include <cassert>

using namespace std;

// public static

string SymValue::toString(SymValue::Kind k) {
	switch(k) {
	case SymValue::Kind::None:		return "None";
	case SymValue::Kind::Variable:	return "Variable";
	case SymValue::Kind::Constant:	return "ConsInt";
	case SymValue::Kind::Procedure:	return "Procedure";
	case SymValue::Kind::Function:	return "Function";
	default:
		assert(false);
	}
}

// public

SymValue::SymValue() : k {Kind::None}, l {0}, t{Datum::Kind::Integer}, n{0} {}

/**
 * @param kind	The token kind, e.g., identifier
 * @param level	The token base/frame level, e.g., 0 for "current frame. Default: 0.
 * @param value The token value, e.g., a procedure address. Default: 0
 * @param fType  The value type. Default Type::Integer 
 * @param nArgs	The number of subroutine parameters. Default 0 
 */
SymValue::SymValue(
	Kind		kind, 
	int 		level,
	Datum 		value,
	Datum::Kind	fType,
	std::size_t	nArgs)

	: k{kind}, l{level}, v{value}, t{fType}, n{nArgs}
{
}

/// @return my current Kind
SymValue::Kind SymValue::kind() const				{	return k;   		}

/// @return my current activation frame lavel
int SymValue::level() const							{	return l;   		}

/**
 * @param value	New value
 * @return My new value.
 */
Datum SymValue::value(Datum value) 					{	return v = value;	}

/// @return my current value
Datum SymValue::value() const						{	return v;			}

/**
 * @param value New formal parameter count
 * @return	My new formal parameter count
 */
size_t SymValue::nArgs(std::size_t value)			{	return n = value;   }

/// @return	My current formal parameter count
size_t SymValue::nArgs() const						{	return n;			}

/**
 * @param value New function return type 
 * @return My function return type
 */
Datum::Kind SymValue::fType(Datum::Kind value)		{	return t = value;	}

/// @return My function return type
Datum::Kind SymValue::fType() const					{	return t;			}
