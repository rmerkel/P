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
 * @brief Constructor
 * @param	name	The fields name
 * @param	type	The fields type, e.g., "integer" or "T"
 ****************************************************************************/
Field::Field(const std::string& name, TDescPtr type)
	: _name{name}, _type{type}
{
}

/********************************************************************************************//**
 * @return my fields name
 ************************************************************************************************/
const std::string& Field::name() const {
	return _name;
}

/********************************************************************************************//**
 * @return my fields type
 ************************************************************************************************/
TDescPtr Field::type() const {
	return _type;
}

// operators

/********************************************************************************************//**
 * @brief Field less-than operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs < rhs
 ************************************************************************************************/
bool operator<(const Field& lhs, const Field& rhs) {
	if (lhs.name() < rhs.name())
		return true;
	else if (lhs.type()	< rhs.type())
		return true;
	else
		return false;
}

/********************************************************************************************//**
 * @brief Field equality operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator==(const Field& lhs, const Field& rhs) {
	return	lhs.name()	== rhs.name()	&&
			lhs.type()	== rhs.type();
}

/********************************************************************************************//**
 * @brief Field stream put operator
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

/********************************************************************************************//**
 * @return my minmum value
 ************************************************************************************************/
int SubRange::minimum() const					{	return _min;		}

/********************************************************************************************//**
 * @return my maximum value
 ************************************************************************************************/
int SubRange::maximum() const					{	return _max;		}

/************************************************************************//**
 * @return the maximum() - minimum() + 1;
 ****************************************************************************/
size_t SubRange::span() const	{
	return maximum() - minimum() + 1;
}

// operators

/********************************************************************************************//**
 * @brief SubRange less-then operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const SubRange& lhs, const SubRange& rhs) {
	if (lhs.minimum() < rhs.minimum())
		return true;
	else if (lhs.maximum() < rhs.minimum())
		return true;
	else
		return false;
}

/********************************************************************************************//**
 * @brief SubRange equality operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator==(const SubRange& lhs, const SubRange& rhs) {
	return	lhs.minimum() == rhs.minimum()	&&
			lhs.maximum() == rhs.minimum();
}

/********************************************************************************************//**
 * @brief Puts value on os
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

// public static

/************************************************************************//**
 * @param	kind		The type kind 
 * @param	size		Size, in bytes, of a object of this type
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @param	rtype		The type array type. Defaults to TDescPtr().
 * @param	base		The type base type. Defaults to TDescPtr().
 * @param	fields		The type fields. Defaults to FieldVec().
 ****************************************************************************/
TDescPtr TDesc::newTDesc(
			TDesc::Kind	kind,
			unsigned	size,
	const	SubRange&	range,
			TDescPtr	rtype,
			TDescPtr	base,
	const	FieldVec&	fields)
{
	return TDescPtr{ new TDesc(kind, size, range, rtype, base, fields) };
}

TDescPtr TDesc::intDesc		= TDesc::newTDesc(
									TDesc::Kind::Integer,
									1,
									SubRange(	numeric_limits<int>::min(),
												numeric_limits<int>::max()));
TDescPtr TDesc::realDesc	= TDesc::newTDesc( TDesc::Kind::Real, 1);
TDescPtr TDesc::charDesc	= TDesc::newTDesc(
									TDesc::Kind::Character,
									1,
									SubRange(0, 127));
TDescPtr TDesc::boolDesc	= TDesc::newTDesc(
									TDesc::Kind::Boolean,
									1,
									SubRange(0, 1));

// public

/************************************************************************//**
 * @param	kind		The type kind 
 * @param	size		Size, in bytes, of a object of this type
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @param	rtype		The type array type. Defaults to TDescPtr().
 * @param	base		The type base type. Defaults to TDescPtr().
 * @param	fields		The type fields. Defaults to FieldVec().
 ****************************************************************************/
TDesc::TDesc(
			TDesc::Kind	kind,
			unsigned	size,
	const	SubRange&	range,
			TDescPtr	rtype,
			TDescPtr	base,
	const	FieldVec&	fields)

	: _kind{kind}, _size(size), _range{range}, _rtype(rtype), _base{base}, _fields{fields}
{
}

/************************************************************************//**
 * @return my kind
 ****************************************************************************/
TDesc::Kind TDesc::kind() const			{	return _kind;		}

/************************************************************************//**
 * @return my size, in bytes
 ****************************************************************************/
unsigned TDesc::size() const			{	return _size;		}

/************************************************************************//**
 * @return my sub-range
 ****************************************************************************/
const SubRange& TDesc::range() const	{	return _range;		}

/************************************************************************//**
 * @return my array index type
 ****************************************************************************/
TDescPtr TDesc::rtype() const			{	return _rtype;		}

/************************************************************************//**
 * @return my base type
 ****************************************************************************/
TDescPtr TDesc::base() const			{	return _base;		}

/************************************************************************//**
 * @return my fields
 ****************************************************************************/
const FieldVec& TDesc::fields() const	{	return _fields;		}

/************************************************************************//**
 * @param flds	My new fields
 ****************************************************************************/
void TDesc::fields(const FieldVec& flds) {
	_fields = flds;
}

/************************************************************************//**
 * @return true if I'm an ordinal type
 ****************************************************************************/
bool TDesc::isOrdinal() const {
	switch(kind()) {
	case Integer:		return true;
	case Boolean:		return true;
	case Character:		return true;
	case Enumeration:	return true;

	default:			return false;
	}
}

/************************************************************************//**
 * @return true if I'm an ordinal type
 ****************************************************************************/
bool TDesc::isElmentary() const {
	switch(kind()) {
	case Integer:		return true;
	case Real:			return true;
	case Boolean:		return true;
	case Character:		return true;

	default:			return false;
	}
}

// operators

/********************************************************************************************//**
 * @brief TDesc less-than operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const TDesc& lhs, const TDesc& rhs) {
	if (lhs.kind() < rhs.kind())
		return true;
	else if (lhs.size()			< rhs.size())
		return true;
	else if (lhs.range()		< rhs.range())
		return true;
	else if (lhs.base()			< rhs.base())
		return true;
	else if (lhs.fields()		< rhs.fields())
		return true;
	else if (lhs.isOrdinal() 	< rhs.isOrdinal())
		return true;
	else if (lhs.isElmentary()	< rhs.isElmentary())
		return true;
	else 
		return false;
}

/********************************************************************************************//**
 * @brief TDesc equality operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator==(const TDesc& lhs, const TDesc& rhs) {
	return	lhs.kind()			== rhs.kind()			&&
			lhs.size()			== rhs.size()			&&
			lhs.range()			== rhs.range()			&&
			lhs.base()			== rhs.base()			&&
			lhs.fields()		== rhs.fields()			&&
			lhs.isOrdinal()		== rhs.isOrdinal()		&&
			lhs.isElmentary()	== rhs.isElmentary();
}

/********************************************************************************************//**
 * @brief TDesc::Kind stream put operator
 *
 * @param	os		Stream to write value on
 * @param	value	Value to write  on os
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const TDesc::Kind& value) {
	switch (value) {
	case TDesc::Integer:		os << "integer";		break;
	case TDesc::Real:			os << "real";			break;
	case TDesc::Boolean:		os << "boolean";		break;
	case TDesc::Character:		os << "character";		break;
	case TDesc::Array:			os << "array";			break;
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
	os 	<<	type.kind()			<< ", "
		<<	type.size()			<< ", "
		<<	type.range()		<< ", "
		<<	type.base()			<< ", ";

	for (auto fld : type.fields())
		os << fld				<< ", ";

	return os
		<<	type.isOrdinal()	<< ", "
		<<	type.isElmentary();
}

