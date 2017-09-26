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

// operators

/********************************************************************************************//**
 * @brief Field stream put operator
 *
 * @param	os		The stream to write field on
 * @param	field	Datum whose value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Field& field) {
	return os << field.name() << ", " << field.type()->kind();
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

// operators

/********************************************************************************************//**
 * Puts value on os
 *
 * @param	os		The stream to write field's value on
 * @param	srange	Value to write 
 * @return	os
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const SubRange& srange) {
	return os << srange.minimum() << ".." << srange.maximum();
}

/************************************************************************//**	
 * Type Descriptor
 ****************************************************************************/

// public

/************************************************************************//**
 * @param	kind	The Type class's kind 
 * @param	size	Size, in bytes, of a object of this type
 * @param	range	The Type class's range. Defaults to SubRange().
 * @param	base	The Type class's base type. Defaults to ConstTDescPtr().
 * @param	fields	The Type classe's fields. Defaults to FieldVec().
 ****************************************************************************/
TDesc::TDesc(
			TDesc::Kind		kind,
			unsigned		size,
	const	SubRange&		range,
			ConstTDescPtr	base,
	const	FieldVec&		fields)

	: _kind{kind}, _size(size), _range{range}, _base{base}, _fields{fields}
{
}

/************************************************************************//**
 * @param	kind	The Type class's kind 
 * @param	size	Size, in bytes, of a object of this type 
 * @param	range	The Type class's range. Default is SubRange().
 * @param	base	The Type class's base type. Default is ConstTDescPtr().
 * @param	fields	The Type classe's fields. Default is FieldVec().
 ****************************************************************************/
TDescPtr TDesc::newTDesc(
			TDesc::Kind		kind,
			unsigned		size,
	const	SubRange&		range,
			ConstTDescPtr	base,
	const	FieldVec&		fields)
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

/************************************************************************//**
 * @return true if I'm an ordinal type
 ****************************************************************************/
bool TDesc::isElmentary() const {
	switch(kind()) {
	case None:			return false;
	case Integer:		return true;
	case Real:			return true;
	case Boolean:		return true;
	case Character:		return true;
	default:			return false;
	}
}

// operators

/********************************************************************************************//**
 * @brief TDesc::Kind stream put operator
 *
 * @param	os		Stream to write value on
 * @param	value	Value to write  on os
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const TDesc::Kind& value) {
	switch (value) {
	case TDesc::None:			os << "unknown";		break;
	case TDesc::Integer:		os << "integer";		break;
	case TDesc::Real:			os << "real";			break;
	case TDesc::Boolean:		os << "boolean";		break;
	case TDesc::Character:		os << "character";		break;
	case TDesc::Array:			os << "array";			break;
	case TDesc::SRange:			os << "subrange";		break;
	case TDesc::Record:			os << "record";			break;
	case TDesc::Enumeration:	os << "enumeration";	break;

	default: 					os << "unknown kind"; assert(false);
	}

	return os;
}

/********************************************************************************************//**
 * @brief TDesc put operator
 *
 * @param	os		The stream to write field's value on
 * @param	type	Value to write 
 * @return	os
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const TDesc& type) {
	os 	<<	type.kind()		<< ", "
		<<	type.size()		<< ", ("
		<<	type.range()	<< "), ("
		<<	*type.base()	<< "), ((";

	const auto flds = type.fields();
	if (flds.size() == 1)
		os << flds[0];
	else
		for (auto& f : flds)
			os << f << ", ";

	return os << "))";
}

