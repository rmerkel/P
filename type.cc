/****************************************************************************
 * @file type.cc
 *
 * Types, both builtin, and created programmatically.
 *
 * Describes both built-in/pre-defined types and user defined types with in
 * the following type kinds (classes):
 *
 * | Kind    | Size | Sub-Range | IType | Base | Fields | Ordinal
 * | :------ | :--: | :-------: | :---: | :--: | :----: | :-----:
 * |  Int	 |	1   | IMIN-IMAX |   -   |   -  |   -    |	Y    
 * |  Real   |	1   |     -     |   -   |   -  |   -    |	N    
 * |  Bool   |	1   |   0 - 1   |   -   |   -  |   -    |	Y    
 * |  Char   |	1   |   0 - 127 |   -   |   -  |   -    |	Y    
 * |  Array  |	N   |    1-10   |  T1   |  T2  |   -    |	N    
 * |  Int    |	1   |    1-10   |   -   |   -  |   -    |	Y    
 * |  Record |	N   |     -     |   -   |   -  | Fields |   N    
 * |  Enum	 |	1   |    X-Y    |   -   |   -  |   -    |	Y    
 * |  Ptr    |  1   |     -     |   -   |   T  |   -    |   N
 *
 * Key:
 * - IType - is the sub-range (index) type for arrays
 * - Base - is the base type for arrays, sub-ranges and enumerations
 * - Fields - is a list of name/type pairs to identify record fields
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

/************************************************************************************************
 * class TypeDesc
 ************************************************************************************************/

// operators

/********************************************************************************************//**
 * @brief TDesc less-than operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const TypeDesc& lhs, const TypeDesc& rhs) {
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
	return	lhs.kind()			== rhs.kind()			&&
			lhs.size()			== rhs.size()			&&
			lhs.range()			== rhs.range()			&&
			lhs.base()			== rhs.base()			&&
			lhs.fields()		== rhs.fields()			&&
			lhs.isOrdinal()		== rhs.isOrdinal();
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
	case TypeDesc::Integer:		os << "Integer";		break;
	case TypeDesc::Real:		os << "Real";			break;
	case TypeDesc::Boolean:		os << "Boolean";		break;
	case TypeDesc::Character:	os << "Char";			break;
	case TypeDesc::Array:		os << "array";			break;
	case TypeDesc::Record:		os << "record";			break;
	case TypeDesc::Enumeration:	os << "Enumeration";	break;
	case TypeDesc::Pointer:		os << "Pointer";		break;

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
ostream& operator<<(std::ostream& os, const TypeDesc& type) {
	os 	<<	type.kind()			<< ", "
		<<	type.size()			<< ", "
		<<	type.range()		<< ", "
		<<	type.base()			<< ", ";

	for (auto fld : type.fields())
		os << fld				<< ", ";

	return os << type.isOrdinal();
};

/************************************************************************************************
 * class TDesc
 ************************************************************************************************/

// public static

SubRange TDesc::maxRange(numeric_limits<int>::min(), numeric_limits<int>::max());

TDescPtr TDesc::intDesc		= TDesc::newIntDesc(maxRange);
TDescPtr TDesc::realDesc	= TDesc::newRealDesc();
TDescPtr TDesc::boolDesc	= TDesc::newBoolDesc();
TDescPtr TDesc::charDesc	= TDesc::newCharDesc(SubRange(0, 127));

/********************************************************************************************//**
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @return TDescPtr to a IntDesc
 ************************************************************************************************/
TDescPtr TDesc::newIntDesc(const SubRange& range) {
	return TDescPtr(new IntDesc(range));
}

/********************************************************************************************//**
 * @return TDescPtr to a new RealDesc
 ************************************************************************************************/
TDescPtr TDesc::newRealDesc()	{ return TDescPtr(new RealDesc());	}

/********************************************************************************************//**
 * @return TDescPtr to a new BoolDesc
 ************************************************************************************************/
TDescPtr TDesc::newBoolDesc()	{ return TDescPtr(new BoolDesc());	}

/********************************************************************************************//**
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @return TDescPtr to a new CharDesc
 ************************************************************************************************/
TDescPtr TDesc::newCharDesc(const SubRange& range) {
	return TDescPtr(new CharDesc(range));
}

/********************************************************************************************//**
 * @param	size		Size, in bytes, of a object of this type
 * @param	range		The type sub-range range.
 * @param	rtype		The type array type.
 * @param	base		The type base type. Defaults to TDescPtr().
 * @return TDescPtr to a new ArrayDesc
 ************************************************************************************************/
TDescPtr TDesc::newArrayDesc(unsigned size, const SubRange& range, TDescPtr rtype, TDescPtr base) {
	return TDescPtr(new ArrayDesc(size, range, rtype, base));
}

/********************************************************************************************//**
 * @param	size		Size, in bytes, of a object of this type
 * @param	fields		The type fields. Defaults to FieldVec().
 * @return TDescPtr to a new RecordDesc
 ************************************************************************************************/
TDescPtr TDesc::newRcrdDesc(unsigned size, const FieldVec& fields) {
	return TDescPtr(new RecordDesc(size, fields));
}

/********************************************************************************************//**
 * @param	range		The type sub-range range.
 * @param	fields		The type fields. Defaults to FieldVec().
 * @return TDescPtr to a new EnumDesc
 ************************************************************************************************/
TDescPtr TDesc::newEnumDesc(const SubRange& range, const FieldVec& fields) {
	return TDescPtr(new EnumDesc(range, fields));
}

/********************************************************************************************//**
 * @param	base		The type base type. Defaults to TDescPtr().
 * @return TDescPtr to a new PtrDesc
 ************************************************************************************************/
TDescPtr TDesc::newPtrDesc(TDescPtr base) {
	return TDescPtr(new PtrDesc(base));
}

