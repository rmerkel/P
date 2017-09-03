/** @file type.cc
 *
 * Types, both builtin, and created by the user.
 */

#include <cassert>
#include <limits>
#include "type.h"

using namespace std;

/****************************************************************************
 * Field - record field name and type pairs
 ****************************************************************************/

// public:

/**
 * @param	name	Name of the field
 * @param	type	Fields type, e.g., "integer" or "T"
 */
Field::Field(const std::string& name, const std::string& type)
	: _name{name}, _type{type}
{
}

/****************************************************************************
 * Sub-Range - minimum to maximum
 ****************************************************************************/

// public

/**
 * @param	minimum	The minimum value in the range
 * @param	maximum	The maximum value in the range
 */
SubRange::SubRange(int minimum, int maximum)
	: _min{minimum}, _max{maximum}
{
}

/**
 * @return the maximum() - minimum() + 1;
 */
size_t SubRange::span() const	{
	return maximum() - minimum() + 1;
}

/************************************************************************//**	
 * Type Descriptor
 ****************************************************************************/

// protected static

	enum Kind {	Integer, Real, Boolean, Character, Array, SRange, Record, Enumeration	};

	/// Return kind as a stirng
const std::string TDesc::toString(Kind kind) {
	switch (kind) {
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

/**
 * @param	kind	The Type class's kind 
 * @param	size	Size, in bytes, of a object of this type
 * @param	range	The Type class's range
 * @param	base	The Type class's base type
 * @param	fields	The Type classe's fields
 */
TDesc::TDesc(
	TDesc::Kind		kind,
	unsigned			size,
	const SubRange&		range,
	const std::string&	base,
	const FieldVec&		fields)

	: _kind{kind}, _size(size), _range{range}, _base{base}, _fields{fields}
{
}

// public

/**
 * @param	kind	The Type class's kind 
 * @param	size	Size, in bytes, of a object of this type 
 * @param	range	The Type class's range
 * @param	base	The Type class's base type
 * @param	fields	The Type classe's fields
 */
TDescPtr TDesc::newTDesc(
	TDesc::Kind		kind,
	unsigned			size,
	const SubRange&		range,
	const std::string&	base,
	const FieldVec&		fields)
{
	return TDescPtr{ new TDesc(kind, size, range, base, fields) };
}

