/****************************************************************************
 * @file type.cc
 *
 * Type system.
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
 * @param	type	The fields type, e.g., "Integer" or "T"
 ****************************************************************************/
Field::Field(const std::string& name, TDescPtr type)
	: _name{name}, _type{type} {
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
	return os << field.name() << ", " << field.type()->tclass();
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
	: _min{minimum}, _max{maximum} {
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
unsigned SubRange::span() const	{
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
			lhs.maximum() == rhs.maximum();
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

/********************************************************************************************//**
 * TypeDesc
 ************************************************************************************************/

// protected:

/**	Construct
 * @param	tclass	My type class
 * @param	size	Object size, in Datums. Defaults to 1.
 * @param	range	Sub-Range, defaults SubRange()
 * @param	itype	Index type. Defaults to TDescPtr()
 * @param	fields	Fields. Defaults to FieldVec()
 * @param	base	Base type. Defaults to TDescPtr().
 * @param	ordinal	True if is an ordinal type.
 */
TypeDesc::TypeDesc(
			TypeClass	tclass,
			unsigned	size,
	const	SubRange&	range,
			TDescPtr	itype,
	const	FieldVec&	fields,
			TDescPtr	base,
			bool		ordinal)

	: _tclass{tclass},
	  _size{size},
	  _range{range},
	  _itype{itype},
	  _fields{fields},
	  _base{base},
	  _ordinal{ordinal}
{}

// public static

SubRange TypeDesc::maxRange(numeric_limits<int>::min(), numeric_limits<int>::max());

TDescPtr TypeDesc::intDesc	= TypeDesc::newIntDesc(maxRange);
TDescPtr TypeDesc::realDesc	= TypeDesc::newRealDesc();
TDescPtr TypeDesc::boolDesc	= TypeDesc::newBoolDesc();
TDescPtr TypeDesc::charDesc	= TypeDesc::newCharDesc(SubRange(0, 127));

/********************************************************************************************//**
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @return TDescPtr to a IntDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newIntDesc(const SubRange& range) {
	return TDescPtr(new TypeDesc(Integer, 1, range, TDescPtr(), FieldVec(), TDescPtr(), true));
}

/********************************************************************************************//**
 * @return TDescPtr to a new RealDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newRealDesc() {
	return TDescPtr(new TypeDesc(Real, 1, SubRange(), TDescPtr(), FieldVec(), TDescPtr(), false));
}

/********************************************************************************************//**
 * @return TDescPtr to a new BoolDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newBoolDesc() {
	return TDescPtr(new TypeDesc(Boolean,
								1,
								SubRange(0, 1),
								TDescPtr(),
								FieldVec(),
								TDescPtr(),
								true));
}

/********************************************************************************************//**
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @return TDescPtr to a new CharDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newCharDesc(const SubRange& range) {
	return TDescPtr(new TypeDesc(Character, 1, range, TDescPtr(), FieldVec(), TDescPtr(), true));
}

/********************************************************************************************//**
 * @param	size		Size, in bytes, of a object of this type
 * @param	range		The type sub-range range.
 * @param	itype		The type array type.
 * @param	base		The type base type. Defaults to TDescPtr().
 * @return TDescPtr to a new ArrayDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newArrayDesc(
			unsigned	size,
	const	SubRange&	range,
			TDescPtr	itype,
			TDescPtr	base)
{
	return TDescPtr(new TypeDesc(Array, size, range, itype, FieldVec(), base, false));
}

/********************************************************************************************//**
 * @param	size		Size, in bytes, of a object of this type
 * @param	fields		The type fields. Defaults to FieldVec().
 * @return TDescPtr to a new RecordDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newRcrdDesc(unsigned size, const FieldVec& fields) {
	return TDescPtr(new TypeDesc(Record,
								size,
								SubRange(),
								TDescPtr(),
								fields,
								TDescPtr(),
								false));
}

/********************************************************************************************//**
 * @param	range		The type sub-range range.
 * @param	fields		The type fields. Defaults to FieldVec().
 * @return TDescPtr to a new EnumDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newEnumDesc(const SubRange& range, const FieldVec& fields) {
	return TDescPtr(new TypeDesc(Enumeration, 1, range, TDescPtr(), fields, TDescPtr(), true));
}

/********************************************************************************************//**
 * @param	base		The type base type. Defaults to TDescPtr().
 * @return TDescPtr to a new PtrDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newPointerDesc(TDescPtr base) {
	return TDescPtr(new TypeDesc(Pointer, 1, SubRange(), TDescPtr(), FieldVec(), base, false));
}

// public

/********************************************************************************************//**
 * @return my type class
 ************************************************************************************************/
TypeDesc::TypeClass TypeDesc::tclass() const	{	return _tclass;					}

/********************************************************************************************//**
 * @return my size, in Datums
 ************************************************************************************************/
unsigned TypeDesc::size() const					{	return _size;					}

/********************************************************************************************//**
 * @return my size, in Datums
 ************************************************************************************************/
unsigned TypeDesc::size(unsigned sz) 			{	return _size = sz;				}

/********************************************************************************************//**
 * @return my sub-range
 ************************************************************************************************/
const SubRange& TypeDesc::range() const 		{	return _range;					}

/********************************************************************************************//**
 * @return my index type
 ************************************************************************************************/
TDescPtr TypeDesc::itype() const 				{	return _itype;					}

/********************************************************************************************//**
 * @return my index type
 ************************************************************************************************/
TDescPtr TypeDesc::itype(TDescPtr type) 		{	return _itype = type;			}

/********************************************************************************************//**
 * @return my fields
 ************************************************************************************************/
const FieldVec& TypeDesc::fields() const		{	return _fields;					}

/********************************************************************************************//**
 * @return my fields
 ************************************************************************************************/
const FieldVec& TypeDesc::fields(const FieldVec& fields) {
	return _fields = fields;
}

/********************************************************************************************//**
 * @return by base type
 ************************************************************************************************/
TDescPtr TypeDesc::base() const 				{	return _base;					}

/********************************************************************************************//**
 * @return by base type
 ************************************************************************************************/
TDescPtr TypeDesc::base(TDescPtr type)			{	return _base = type;			}

/********************************************************************************************//**
 * @return true if my type is ordinal
 ************************************************************************************************/
bool TypeDesc::isOrdinal() const 				{	return _ordinal;				}

// operators

/********************************************************************************************//**
 * @brief TDesc less-than operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const TypeDesc& lhs, const TypeDesc& rhs) {
	if (lhs.tclass()			< rhs.tclass())
		return true;
	else if (lhs.size()			< rhs.size())
		return true;
	else if (lhs.range()		< rhs.range())
		return true;
	else if (lhs.base()			< rhs.base())
		return true;
	else if (lhs.fields()		< rhs.fields())
		return true;
	else if (lhs.isOrdinal()	< rhs.isOrdinal())
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
bool operator==(const TypeDesc& lhs, const TypeDesc& rhs) {
	return	lhs.tclass()		== rhs.tclass()		&&
			lhs.size()			== rhs.size()		&&
			lhs.range()			== rhs.range()		&&
			lhs.base()			== rhs.base()		&&
			lhs.fields()		== rhs.fields()		&&
			lhs.isOrdinal()		== rhs.isOrdinal();
}

/********************************************************************************************//**
 * @brief TypeClass put operator
 *
 * @param	os		The stream to write type class value on
 * @param	tclass	Value to write 
 * @return	os
 ************************************************************************************************/
std::ostream& operator<<(std::ostream& os, TypeDesc::TypeClass tclass) {
	switch(tclass) {
	case TypeDesc::Array:		os << "array";			break;
	case TypeDesc::Boolean:		os << "boolean";		break;
	case TypeDesc::Character:	os << "character";		break;
	case TypeDesc::Enumeration:	os << "enumeration";	break;
	case TypeDesc::Integer:		os << "integer";		break;
	case TypeDesc::Pointer:		os << "pointer";		break;
	case TypeDesc::Real:		os << "real";			break;
	case TypeDesc::Record:		os << "record";			break;
	case TypeDesc::Set:			os << "set";			break;
	default:
		os << "unknown (" << static_cast<unsigned>(tclass) << ")";
		assert(false);
	}

	return os;
}

/********************************************************************************************//**
 * @brief TDesc put operator
 *
 * @param	os		The stream to write field's value on
 * @param	tdesc	Value to write 
 * @return	os
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const TypeDesc& tdesc) {
	os 	<<	tdesc.tclass()	<< ", "
		<<	tdesc.size()	<< ", "
		<<	tdesc.range()	<< ", "
		<<  tdesc.itype()	<< ", "
		<<	tdesc.base()	<< ", ";

	for (auto fld : tdesc.fields())
		os << fld			<< ", ";

	return os 				<< tdesc.isOrdinal();
};

