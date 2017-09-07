/****************************************************************************
 * @file type.cc
 *
 * Types, both builtin, and created by programmatically.
 ****************************************************************************/

#include <cassert>
#include <limits>
#include "type.h"

using namespace std;

/****************************************************************************
 * Field - record field name and type pairs
 ****************************************************************************/

// public:

/************************************************************************//**
 * @param	name	Name of the field
 * @param	type	Fields type, e.g., "integer" or "T"
 ****************************************************************************/
Field::Field(const std::string& name, ConstTDescPtr type)
	: _name{name}, _type{type}
{
}

/****************************************************************************
 * Sub-Range - minimum to maximum
 ****************************************************************************/

// public

/************************************************************************//**
 * @param	minimum	The minimum value in the range
 * @param	maximum	The maximum value in the range
 ****************************************************************************/
SubRange::SubRange(int minimum, int maximum)
	: _min{minimum}, _max{maximum}
{
}

/************************************************************************//**
 * @return the maximum() - minimum() + 1;
 ****************************************************************************/
size_t SubRange::span() const	{
	return maximum() - minimum() + 1;
}

/************************************************************************//**	
 * Type Descriptor
 ****************************************************************************/

// protected static

/************************************************************************//**
 * Return kind as a stirng
 ****************************************************************************/
const std::string TDesc::toString(Kind kind) {
	switch (kind) {
	case None:			return "unknown";
	case Integer:		return "integer";
	case Real:			return "real";
	case Boolean:		return "boolean";
	case Character:		return "character";
	case Array:			return "array";
	case SRange:		return "subrange";
	case Record:		return "record";
	case Enumeration:	return "enumeration";
	default:
		assert(0);
		return "unknown!";
	}
}

// protected:

/************************************************************************//**
 * @param	kind	The Type class's kind 
 * @param	size	Size, in bytes, of a object of this type
 * @param	range	The Type class's range
 * @param	base	The Type class's base type
 * @param	fields	The Type classe's fields
 ****************************************************************************/
TDesc::TDesc(
	TDesc::Kind		kind,
	unsigned		size,
	const SubRange&	range,
	ConstTDescPtr	base,
	const FieldVec&	fields)

	: _kind{kind}, _size(size), _range{range}, _base{base}, _fields{fields}
{
}

// public

/************************************************************************//**
 * @param	kind	The Type class's kind 
 * @param	size	Size, in bytes, of a object of this type 
 * @param	range	The Type class's range
 * @param	base	The Type class's base type
 * @param	fields	The Type classe's fields
 ****************************************************************************/
TDescPtr TDesc::newTDesc(
	TDesc::Kind		kind,
	unsigned		size,
	const SubRange&	range,
	ConstTDescPtr	base,
	const FieldVec&	fields)
{
	return TDescPtr{ new TDesc(kind, size, range, base, fields) };
}

/************************************************************************//**
 * @return true if I'm an ordinal type
 ****************************************************************************/
bool TDesc::isOrdinal() const {
	switch(kind()) {
	case None:			return false;
	case Integer:		return true;
	case Real:			return false;
	case Boolean:		return true;
	case Character:		return true;
	case Array:			return false;
	case SRange:		return true;
	case Record:		return false;
	case Enumeration:	return true;
	default:			return false;
	}
}

