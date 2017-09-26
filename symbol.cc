/********************************************************************************************//**
 * @file	symbol.cc
 *
 * The Pascal-Lite Symbol table implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include <cassert>

#include "symbol.h"

using namespace std;

// public

/********************************************************************************************//**
 * @note kind() == None, i.e., a place holder for a real symbol
 ************************************************************************************************/
SymValue::SymValue() : _kind{Kind::None}, _level{0} {}

/********************************************************************************************//**
 * Constants have a integer value, data type (descriptor), and an active frame/block level. 
 *
 * @param level	The base/frame level, e.g., 0 for the current frame.
 * @param value The constant data value.
 * @param type	Type descriptor. Assumed to be for "integer"
 ************************************************************************************************/
SymValue::SymValue(int level, Datum value, TDescPtr type)
	: _kind{SymValue::Constant}, _level{level}, _value{value}, _type(type)
{
}

/********************************************************************************************//**
 * Variables have a type, location, as an offset from a block/frame, n levels down.
 *
 * @param level		The base/frame level, e.g., 0 for "current frame..
 * @param offset	The variables location as a ofset from the activation frame
 * @param type  	the variables type descriptor
 ************************************************************************************************/
SymValue::SymValue(int level, int offset, TDescPtr type)
	: _kind{SymValue::Variable}, _level{level}, _value{offset}, _type{type}
{
}

/********************************************************************************************//**
 * Constructs a partially defined Function or Procedure. The entry point address and return type,
 * Function only, has to be set later. 
 *
 * @invariant kind must be equal to Procedure or Function
 *
 * @param kind	The token kind, Procedure or Function.
 * @param level	The token base/frame level, e.g., 0 for "current frame.
 ************************************************************************************************/
SymValue::SymValue(Kind kind, int level) : _kind{kind}, _level{level}, _value{0}
{
	assert(SymValue::Procedure == _kind || SymValue::Function == _kind);
}


/********************************************************************************************//**
 * Types have a block/frame level, and of course, a type descriptor.
 ************************************************************************************************/
SymValue::SymValue(int level, TDescPtr type)
	: _kind{Kind::Type}, _level{level}, _value{0}, _type{type}
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
ConstTDescPtr SymValue::type(TDescPtr type)			{	return _type = type;	}

/********************************************************************************************//**
 * @return My type
 ************************************************************************************************/
ConstTDescPtr SymValue::type() const				{	return _type;			}

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

