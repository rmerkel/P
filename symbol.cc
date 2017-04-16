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
	case SymValue::None:		return "None";
	case SymValue::Variable:	return "Variable";
	case SymValue::ConstInt:	return "ConsInt";
	case SymValue::ConstReal:	return "ConstReal";
	case SymValue::Procedure:	return "Procedure";
	case SymValue::Function:	return "Function";
	default:
		assert(false);
	}
}

// public

SymValue::SymValue() : k {None}, l {0}, n{0} {}

/**
 * @param kind	The token kind, e.g., identifier
 * @param level	The token base/frame level, e.g., 0 for "current frame. Default: 0.
 * @param value The token value, e.g., a procedure address. Default: 0
 * @param nArgs	The number of subroutine parameters. Default 0
 */
SymValue::SymValue(Kind kind, int level, pl0c::Datum value, std::size_t nArgs)
	: k{kind}, l{level}, v{value}, n{nArgs}			{}

/// @return my current Kind
SymValue::Kind SymValue::kind() const				{	return k;   		}

/// @return my current activation frame lavel
int SymValue::level() const							{	return l;   		}

/**
 * @param value	New value
 * @return My new value.
 */
pl0c::Datum SymValue::value(pl0c::Datum value) 		{	return v = value;	}

/// @return my current value
pl0c::Datum SymValue::value() const					{	return v;			}

/**
 * @param value New formal parameter count
 * @return	My new formal parameter count
 */
std::size_t SymValue::nArgs(std::size_t value)		{	return n = value;   }

/// @return	My current formal parameter count
std::size_t SymValue::nArgs() const					{	return n;			}
