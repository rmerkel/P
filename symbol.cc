/********************************************************************************************//**
 * @file	symbol.cc
 *
 * The P language Symbol table implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include <cassert>

#include "symbol.h"

using namespace std;

// public static

/********************************************************************************************//**
 * @brief return a constant symbol value
 *
 * Constants have a integer value, data type (descriptor), and an active frame/block level. 
 *
 * @param level	The base/frame level, e.g., 0 for the current frame.
 * @param value The constant data value.
 * @param type	Type descriptor. Assumed to be for "Integer"
 *
 * @return a constant symbol value
 ************************************************************************************************/
SymValue SymValue::makeConst(int level, const Datum& value, TDescPtr type) {
	return SymValue(SymValue::Constant, level, value, type, TDescPtrVec());
}

/********************************************************************************************//**
 * @brief return a variable symbol value
 *
 * Variables have a type, location, as an offset from a block/frame, n levels down.
 *
 * @param level		The base/frame level, e.g., 0 for "current frame..
 * @param offset	The variables location as a ofset from the activation frame
 * @param type  	the variables type descriptor
 *
 * @return a variable symbol value
 ************************************************************************************************/
SymValue SymValue::makeVar(int level, int offset, TDescPtr type) {
	return SymValue(SymValue::Variable, level, Datum(offset), type, TDescPtrVec());
}

/********************************************************************************************//**
 * @brief return a partially defined sub-routine.
 *
 * The entry point address and return type, function only, has to be set later. 
 *
 * @invariant kind must be equal to Procedure or Function
 *
 * @param kind	The token kind, Procedure or Function.
 * @param level	The token base/frame level, e.g., 0 for "current frame.
 * 
 * @return a partial sub-routine symbol value
 ************************************************************************************************/
SymValue SymValue::makeSbr(Kind kind, int level) {
	assert(SymValue::Procedure == kind || SymValue::Function == kind);
	return SymValue(kind, level, Datum(0), TDescPtr(), TDescPtrVec());
}

/********************************************************************************************//**
 * @brief return a type symbol value
 *
 * Types have a block/frame level, and of course, a type descriptor.
 *
 * @param level	The token base/frame level, e.g., 0 for "current frame.
 * @param type type description
 *
 * @return a type symbol value
 ************************************************************************************************/
SymValue SymValue::makeType(int level, TDescPtr type) {
	return SymValue(Kind::Type, level, Datum(0), type, TDescPtrVec());
}

// public

/********************************************************************************************//**
 * @note kind() == None, i.e., a place holder for a real symbol
 ************************************************************************************************/
SymValue::SymValue() : _kind{Kind::None}, _returned(false), _level{0} {}

/********************************************************************************************//**
 * @brief Construct a symbol value from it's components
 *
 * @param kind		The symbol kind, e.g., Constant, Variable, etc...
 * @param level		The base/frame level, e.g., 0 for the current frame.
 * @param value 	The symbols data value, variable offset, etc...
 * @param type		The symbol data value type
 * @param params	SubRoutine parameter type array
 ************************************************************************************************/
SymValue::SymValue(Kind kind, int level, const Datum& value, TDescPtr type, const TDescPtrVec& params)
	: _kind{kind}, _returned(false), _level{level}, _value{value}, _type{type}, _params{params}
{
}

/********************************************************************************************//**
 * @return my Kind
 ************************************************************************************************/
SymValue::Kind SymValue::kind() const				{	return _kind;  			}

/********************************************************************************************//**
 * @return my current activation frame lavel
 ************************************************************************************************/
int SymValue::level() const							{	return _level; 			}

/********************************************************************************************//**
 * @return ny new returned value
 ************************************************************************************************/
bool SymValue::returned(bool r)						{	return _returned = r;	}

/********************************************************************************************//**
 * @return my current returned value
 ************************************************************************************************/
bool SymValue::returned() const						{	return _returned;		}

/********************************************************************************************//**
 * @param value	New value
 * @return My new value.
 ************************************************************************************************/
Datum SymValue::value(Datum value) 					{	return _value = value;	}

/********************************************************************************************//**
 * @return my value
 ************************************************************************************************/
Datum SymValue::value() const						{	return _value;			}

/********************************************************************************************//**
 * @param type	New symbol type
 * @return My function return type
 ************************************************************************************************/
TDescPtr SymValue::type(TDescPtr type)				{	return _type = type;	}

/********************************************************************************************//**
 * @return My type
 ************************************************************************************************/
TDescPtr SymValue::type() const						{	return _type;			}

/********************************************************************************************//**
 * Return subrountine paramer kinds, in order of declaractions.
 *
 * @return Subrountine kinds
 ************************************************************************************************/
TDescPtrVec& SymValue::params() 					{   return _params;			}

/********************************************************************************************//**
 * Return subrountine paramer kinds, in order of declaractions
 *
 * @return Subrountine kinds
 ************************************************************************************************/
const TDescPtrVec& SymValue::params() const 		{   return _params;			}

// operators

/********************************************************************************************//**
 * @brief Datum::Kind stream put operator
 *
 * Puts Datum value on os per it's discriminator. 
 *
 * @param	os		Stream to write d's value to 
 * @param	kind	Datum kind whose value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const SymValue::Kind& kind) {
	switch(kind) {
	case SymValue::None:		os << "None";		break;
	case SymValue::Variable:	os << "Variable";	break;
	case SymValue::Constant:	os << "ConsInt";	break;
	case SymValue::Procedure:	os << "Procedure";	break;
	case SymValue::Function:	os << "Function";	break;
	case SymValue::Type:		os << "Type";		break;
	default:
		assert(false);
		os << "Unknown SymValue Kind!";
	}
	
	return os;
}

