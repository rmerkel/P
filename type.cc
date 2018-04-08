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

/********************************************************************************************//**
 * TypeDesc
 ************************************************************************************************/

// protected:

/**	Construct
 * @param	tclass	My type class
 * @param	size	Object size, in Datums. Defaults to 1.
 * @param	range	Subrange, defaults Subrange()
 * @param	itype	Index type. Defaults to TDescPtr()
 * @param	fields	Fields. Defaults to FieldVec()
 * @param	base	Base type. Defaults to TDescPtr().
 * @param	ordinal	True if is an ordinal type.
 * @param	ref		True if objects of this type are passed by reference
 */
TypeDesc::TypeDesc(
			TypeClass	tclass,
			size_t		size,
	const	Subrange&	range,
			TDescPtr	itype,
	const	FieldVec&	fields,
			TDescPtr	base,
			bool		ordinal,
			bool		ref)

	: _tclass{tclass},
	  _size{size},
	  _range{range},
	  _itype{itype},
	  _fields{fields},
	  _base{base},
	  _ordinal{ordinal},
	  _ref{ref}
{}

// public static

Subrange TypeDesc::maxRange = Subrange::maxRange;
Subrange TypeDesc::charRange(0, 127);

/********************************************************************************************//**
 * @param	range		The type sub-range range. Defaults to Subrange().
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a IntDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newIntDesc(const Subrange& range, bool ref) {
	return TDescPtr(new TypeDesc(Integer,
								1,
								range,
								TDescPtr(),
								FieldVec(),
								TDescPtr(),
								true,
								ref));
}

/********************************************************************************************//**
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new RealDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newRealDesc(bool ref) {
	return TDescPtr(new TypeDesc(Real,
								1,
								Subrange(),
								TDescPtr(),
								FieldVec(),
								TDescPtr(),
								false,
								ref));
}

/********************************************************************************************//**
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new BoolDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newBoolDesc(bool ref) {
	return TDescPtr(new TypeDesc(Boolean,
								1,
								Subrange(0, 1),
								TDescPtr(),
								FieldVec(),
								TDescPtr(),
								true,
								ref));
}

/********************************************************************************************//**
 * @param	range		The type sub-range range. Defaults to charRange.
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new CharDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newCharDesc(const Subrange& range, bool ref) {
	return TDescPtr(new TypeDesc(Character,
								1,
								range,
								TDescPtr(),
								FieldVec(),
								TDescPtr(),
								true,
								ref));
}

/********************************************************************************************//**
 * @param	size		Size, in bytes, of a object of this type
 * @param	range		The type sub-range range.
 * @param	itype		The type array type.
 * @param	base		The type base type. Defaults to TDescPtr().
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new ArrayDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newArrayDesc(
			size_t		size,
	const	Subrange&	range,
			TDescPtr	itype,
			TDescPtr	base,
			bool		ref)
{
	return TDescPtr(new TypeDesc(Array, size, range, itype, FieldVec(), base, false, ref));
}

/********************************************************************************************//**
 * @param	size		Size, in bytes, of a object of this type
 * @param	fields		The type fields. Defaults to FieldVec().
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new RecordDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newRcrdDesc(size_t size, const FieldVec& fields, bool ref) {
	return TDescPtr(new TypeDesc(Record,
								size,
								Subrange(),
								TDescPtr(),
								fields,
								TDescPtr(),
								false,
								ref));
}

/********************************************************************************************//**
 * @param	range		The type sub-range range.
 * @param	fields		The type fields. Defaults to FieldVec().
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new EnumDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newEnumDesc(const Subrange& range, const FieldVec& fields, bool ref) {
	return TDescPtr(new TypeDesc(Enumeration,
								1,
								range,
								TDescPtr(),
								fields,
								TDescPtr(),
								true,
								ref));
}

/********************************************************************************************//**
 * @param	base		The type base type. Defaults to TDescPtr().
 * @param	ref			Type is passed by reference
 *
 * @return TDescPtr to a new PtrDesc
 ************************************************************************************************/
TDescPtr TypeDesc::newPointerDesc(TDescPtr base, bool ref) {
	return TDescPtr(new TypeDesc(Pointer,
								1,
								Subrange(),
								TDescPtr(),
								FieldVec(),
								base,
								false,
								ref));
}

// public

/********************************************************************************************//**
 * @return my type class
 ************************************************************************************************/
TypeDesc::TypeClass TypeDesc::tclass() const	{	return _tclass;					}

/********************************************************************************************//**
 * @return my size, in Datums
 ************************************************************************************************/
size_t TypeDesc::size() const					{	return _size;					}

/********************************************************************************************//**
 * @return my size, in Datums
 ************************************************************************************************/
size_t TypeDesc::size(size_t sz) 				{	return _size = sz;				}

/********************************************************************************************//**
 * @return my sub-range
 ************************************************************************************************/
const Subrange& TypeDesc::range() const 		{	return _range;					}

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
 * @param	type	My new base type
 * @return type
 ************************************************************************************************/
TDescPtr TypeDesc::base(TDescPtr type)			{	return _base = type;			}

/********************************************************************************************//**
 * @return true if my type is ordinal
 ************************************************************************************************/
bool TypeDesc::ordinal() const 					{	return _ordinal;				}

/********************************************************************************************//**
 * @return true if my type is passed by reference
 ************************************************************************************************/
bool TypeDesc::ref() const						{	return _ref;					}

/********************************************************************************************//**
 * @param	r	True if my type is passed by refernce
 * @return	r
 ************************************************************************************************/
bool TypeDesc::ref(bool r)						{	return _ref = r;				}

// operators

/********************************************************************************************//**
 * @brief TDesc less-than operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const TypeDesc& lhs, const TypeDesc& rhs) {
	if (lhs.tclass()		< rhs.tclass())
		return true;
	else if (lhs.size()		< rhs.size())
		return true;
	else if (lhs.range()	< rhs.range())
		return true;
	else if (lhs.base()		< rhs.base())
		return true;
	else if (lhs.fields()	< rhs.fields())
		return true;
	else if (lhs.ordinal()	< rhs.ordinal())
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
	return	lhs.tclass()	== rhs.tclass()		&&
			lhs.size()		== rhs.size()		&&
			lhs.range()		== rhs.range()		&&
			lhs.base()		== rhs.base()		&&
			lhs.fields()	== rhs.fields()		&&
			lhs.ordinal()	== rhs.ordinal();
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

	return os 				<< tdesc.ordinal();
};

