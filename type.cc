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
 * class iTDesc
 ************************************************************************************************/

// operators

/********************************************************************************************//**
 * @brief TDesc less-than operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const iTDesc& lhs, const iTDesc& rhs) {
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
bool operator==(const iTDesc& lhs, const iTDesc& rhs) {
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
	case iTDesc::Integer:		os << "Integer";		break;
	case iTDesc::Real:			os << "Real";			break;
	case iTDesc::Boolean:		os << "Boolean";		break;
	case iTDesc::Character:		os << "Char";			break;
	case iTDesc::Array:			os << "array";			break;
	case iTDesc::Record:		os << "record";			break;
	case iTDesc::Enumeration:	os << "Enumeration";	break;

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
ostream& operator<<(std::ostream& os, const iTDesc& type) {
	os 	<<	type.kind()			<< ", "
		<<	type.size()			<< ", "
		<<	type.range()		<< ", "
		<<	type.base()			<< ", ";

	for (auto fld : type.fields())
		os << fld				<< ", ";

	return os << type.isOrdinal();
};

/********************************************************************************************//**
 * Type Descriptor base
 *
 * TypeDesc's have a sub-range and a fields vector
 ************************************************************************************************/
class TypeDesc : public iTDesc {
public:
	typedef iTDesc::Kind	Kind;			///< iTDesc::Kind alias

	/// Constructor
	TypeDesc(const SubRange& range = SubRange(), const FieldVec& fields = FieldVec())
		: _range{range}, _fields{fields}	{}

protected:
	/// Return my sub-range
	const SubRange& range() const override	{	return _range;	}

	/// Set, and return, my sub-range
	const SubRange& range(const SubRange& range) override {
		return _range = range;
	}

	/// Return my fields
	const FieldVec& fields() const override	{	return _fields;	}

	/// Set, and return, my fields
	const FieldVec& fields(const FieldVec& fields) override {
		return _fields = fields;
	}

	SubRange	_range;						///< My sub-range. For arrays, the array's span
	FieldVec	_fields;					///< My fields (enumeration and record)
};

/********************************************************************************************//**
 * Integer, including sub-ranges, type descriptor
 ************************************************************************************************/
class IntDesc : public TypeDesc {
public:
	/// Construct an IntDesc from it's sub-range
	IntDesc(const SubRange& range) : TypeDesc{range} {}

	Kind kind() const override				{	return iTDesc::Integer;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Integer; 	}
	unsigned size() const override			{	return 1;								}
	unsigned size(unsigned) override		{	assert(false); return 1; 				}
	TDescPtr rtype() const override			{	return TDescPtr();						}
	TDescPtr rtype(TDescPtr) override		{	assert(false); return TDescPtr();		}
	TDescPtr base() const override			{	return TDescPtr();						}
	TDescPtr base(TDescPtr) override		{	assert(false); return TDescPtr();		}
	bool isOrdinal() const override			{	return true;							}
};

/********************************************************************************************//**
 * Real type descriptor
 ************************************************************************************************/
class RealDesc : public TypeDesc {
public:
	/// Construct a RealDesc
	RealDesc() : TypeDesc()					{}

	Kind kind() const override				{	return iTDesc::Real;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Real; 	}
	unsigned size() const override			{	return 1;								}
	unsigned size(unsigned) override		{	assert(false); return 1; 				}
	TDescPtr rtype() const override			{	return TDescPtr();						}
	TDescPtr rtype(TDescPtr) override		{	assert(false); return TDescPtr();		}
	TDescPtr base() const override			{	return TDescPtr();						}
	TDescPtr base(TDescPtr) override		{	assert(false); return TDescPtr();		}
	bool isOrdinal() const override			{	return false;							}
};

/********************************************************************************************//**
 * Boolean type descriptor
 ************************************************************************************************/
class BoolDesc : public TypeDesc {
public:
	/// Construct a BoolDesc
	BoolDesc() : TypeDesc()					{}

	Kind kind() const override				{	return iTDesc::Boolean;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Boolean; 	}
	unsigned size() const override			{	return 1;								}
	unsigned size(unsigned) override		{	assert(false); return 1; 				}
	TDescPtr rtype() const override			{	return TDescPtr();						}
	TDescPtr rtype(TDescPtr) override		{	assert(false); return TDescPtr();		}
	TDescPtr base() const override			{	return TDescPtr();						}
	TDescPtr base(TDescPtr) override		{	assert(false); return TDescPtr();		}
	bool isOrdinal() const override			{	return true;							}
};


/********************************************************************************************//**
 * Character type descriptor
 ************************************************************************************************/
class CharDesc : public TypeDesc {
public:
	/// Construct a CharDes from its sub-range
	CharDesc(const SubRange& range) : TypeDesc{range} {}

	Kind kind() const override				{	return iTDesc::Character;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Character; 	}
	unsigned size() const override			{	return 1;									}
	unsigned size(unsigned) override		{	assert(false); return 1; 					}
	TDescPtr rtype() const override			{	return TDescPtr();							}
	TDescPtr rtype(TDescPtr) override		{	assert(false); return TDescPtr();			}
	TDescPtr base() const override			{	return TDescPtr();							}
	TDescPtr base(TDescPtr) override		{	assert(false); return TDescPtr();			}
	bool isOrdinal() const override			{	return true;								}
};

/********************************************************************************************//**
 * Array type descriptor
 ************************************************************************************************/
class ArrayDesc : public TypeDesc {
public:
	/// Construct an ArrayDesc from it's size, index, type and base type
	ArrayDesc(unsigned size, const SubRange& index, TDescPtr rtype, TDescPtr base)
		: TypeDesc(index), _size{size}, _rtype{rtype}, _base{base} {}

	Kind kind() const override				{	return iTDesc::Array;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Array; 	}
	unsigned size() const override			{	return _size;							}
	unsigned size(unsigned sz) override		{	return _size = sz; 						}
	TDescPtr rtype() const override			{	return _rtype;							}
	TDescPtr rtype(TDescPtr type) override 	{	return _rtype = type;					}
	TDescPtr base() const override			{	return _base;							} 
	TDescPtr base(TDescPtr type) override 	{	return _base = type;					}
	bool isOrdinal() const override			{	return false;							}

private:
	unsigned	_size;						///< Size of on object of my type, in Datums
	TDescPtr	_rtype;						///< Arrays index (sub-range) type
	TDescPtr	_base;						///< Arrays base type
};

/********************************************************************************************//**
 * Record type descriptor
 ************************************************************************************************/
class RcrdDesc : public TypeDesc {
public:
	/// Construct a RcrdDesc from it's size ,and fields
	RcrdDesc(unsigned sz, const FieldVec& fields)
		: TypeDesc(SubRange(), fields), _size(sz) {}

	Kind kind() const override				{	return iTDesc::Record;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Record; 	}
	unsigned size() const override			{	return _size;							}
	unsigned size(unsigned sz) override		{	return _size = sz; 						}
	TDescPtr rtype() const override			{	return TDescPtr();						}
	TDescPtr rtype(TDescPtr) override		{	assert(false); return TDescPtr();		}
	TDescPtr base() const override			{	return TDescPtr();						}
	TDescPtr base(TDescPtr) override		{	assert(false); return TDescPtr();		}
	bool isOrdinal() const override			{	return false;							}

private:
	unsigned	_size;						///< Size of on object of my type, in Datums
};

 
/********************************************************************************************//**
 * Enumeration type descptor
 ************************************************************************************************/
class EnumDesc : public TypeDesc {
public:
	/// Construct a EnumDesc from it's range and fields
	EnumDesc(const SubRange& range, const FieldVec& fields)
		: TypeDesc(range, fields) 			{}

	Kind kind() const override				{	return iTDesc::Enumeration;					}
	Kind kind(Kind) override				{	assert(false); return iTDesc::Enumeration; 	}
	unsigned size() const override			{	return 1;								}
	unsigned size(unsigned) override		{	assert(false); return 1; 				}
	TDescPtr rtype() const override			{	return TDescPtr();						}
	TDescPtr rtype(TDescPtr) override		{	assert(false); return TDescPtr();		}
	TDescPtr base() const override			{	return TDescPtr();						}
	TDescPtr base(TDescPtr) override		{	assert(false); return TDescPtr();		}
	bool isOrdinal() const override			{	return true;							}
};

/************************************************************************************************
 * class TDesc
 ************************************************************************************************/

// public static

SubRange TDesc::maxRange(numeric_limits<int>::min(), numeric_limits<int>::max());

TDescPtr TDesc::intDesc		= TDesc::newTDesc(TDesc::Kind::Integer, 1, maxRange);
TDescPtr TDesc::realDesc	= TDesc::newTDesc(TDesc::Kind::Real, 1);
TDescPtr TDesc::charDesc	= TDesc::newTDesc(TDesc::Kind::Character, 1, SubRange(0, 127));
TDescPtr TDesc::boolDesc	= TDesc::newTDesc(TDesc::Kind::Boolean, 1, SubRange(0, 1));

/********************************************************************************************//**
 * @param	kind		The type kind 
 * @param	size		Size, in bytes, of a object of this type
 * @param	range		The type sub-range range. Defaults to SubRange().
 * @param	rtype		The type array type. Defaults to TDescPtr().
 * @param	base		The type base type. Defaults to TDescPtr().
 * @param	fields		The type fields. Defaults to FieldVec().
 ************************************************************************************************/
TDescPtr TDesc::newTDesc(
			TDesc::Kind	kind,
			unsigned	size,
	const	SubRange&	range,
			TDescPtr	rtype,
			TDescPtr	base,
	const	FieldVec&	fields)
{
	switch(kind) {
	case iTDesc::Integer:	return TDescPtr{ new IntDesc(range)						};	break;
	case iTDesc::Real:		return TDescPtr{ new RealDesc()							};	break;
	case iTDesc::Boolean:	return TDescPtr{ new BoolDesc()							};	break;
	case iTDesc::Character:	return TDescPtr{ new CharDesc(range)					};	break;
	case iTDesc::Array:		return TDescPtr{ new ArrayDesc(size,range,rtype,base)	};	break;
	case iTDesc::Record:	return TDescPtr{ new RcrdDesc(size, fields)				};	break;
	case iTDesc::Enumeration: return TDescPtr{ new EnumDesc(range, fields)			};	break;
	default:				assert(false);
	}
}

