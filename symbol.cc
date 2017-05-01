/**	@file	symbol.cc
 *
 * The PL/0C Symbol table implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "symbol.h"

#include <cassert>

using namespace std;

// public static

/// @param	k	Symbol table Kind show name we return
string SymValue::toString(SymValue::Kind k) {
	switch(k) {
	case SymValue::Kind::None:		return "None";
	case SymValue::Kind::Variable:	return "Variable";
	case SymValue::Kind::Constant:	return "ConsInt";
	case SymValue::Kind::Procedure:	return "Procedure";
	case SymValue::Kind::Function:	return "Function";
	default:
		assert(false);
		return "Unknown SymValue Kind!";
	}
}

// public

SymValue::SymValue() : k {Kind::None}, l {0}, t{Datum::Kind::Integer}, n{0} {}

/** 
 * Constants have a data value, value and a active frame/block level. 
 * @param level	The base/frame level, e.g., 0 for "current frame.
 * @param value The constant data value.
 */
SymValue::SymValue(int level, Datum value)
	: k{SymValue::Kind::Constant}, l{level}, v{value}, t{v.k}, n{0}
{
}

/** 
 * Variables don't have a data value, but do have a data type, location as a 
 * offset from a activaation frame/block, n levels down 
 * @param level		The base/frame level, e.g., 0 for "current frame..
 * @param offset	The variables location as a ofset from the activation frame
 * @param type  	the variables type, e.g., Datum::Kind::Integer.
 */
SymValue::SymValue(int level, Datum::Integer offset, Datum::Kind type)
	: k{SymValue::Kind::Variable}, l{level}, v{offset}, t{type}, n{0}
{
}

/** 
 * Constructs a partially defined function or procedure. The offset and 
 * (Function) return type has to be set later. 
 * @param kind	The token kind, e.g., identifier
 * @param level	The token base/frame level, e.g., 0 for "current frame.
 */
SymValue::SymValue(Kind kind, int level)
	: k{kind}, l{level}, v{0}, t{Datum::Kind::Integer}, n{0}
{
	assert(SymValue::Kind::Procedure == k || SymValue::Kind::Function == k);
}

/// @return my Kind
SymValue::Kind SymValue::kind() const				{	return k;   		}

/// @return my current activation frame lavel
int SymValue::level() const							{	return l;   		}

/**
 * @param value	New value
 * @return My new value.
 */
Datum SymValue::value(Datum value) 					{	return v = value;	}

/// @return my value
Datum SymValue::value() const						{	return v;			}

/**
 * @param value New formal parameter count
 * @return	My new formal parameter count
 */
size_t SymValue::nArgs(std::size_t value)			{	return n = value;   }

/// @return	My formal parameter count
size_t SymValue::nArgs() const						{	return n;			}

/**
 * @param value New symbol type
 * @return My function return type
 */
Datum::Kind SymValue::type(Datum::Kind value)		{	return t = value;	}

/// @return My type
Datum::Kind SymValue::type() const					{	return t;			}

