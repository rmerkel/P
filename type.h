/************************************************************************//**
 * @file type.h
 *
 * Pascal-lite type system. Type descriptors, record fields and sub-ranges.
 ****************************************************************************/

#ifndef	TYPE_H
#define	TYPE_H

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>

class TypeDesc; 

/************************************************************************//**	
 * Pointer to TypeDesc
 ****************************************************************************/
typedef std::shared_ptr<TypeDesc>		TDescPtr;

/************************************************************************//**	
 * Vector of TDescPtrs
 ****************************************************************************/
typedef std::vector<TDescPtr>			TDescPtrVec;

/************************************************************************//**
 * Type Field - record field name and type pairs
 ****************************************************************************/
class Field {
	std::string	_name;					///< The fields name
	TDescPtr	_type;					///< The fields type

public:
	Field() {}							///< Default constructor
	Field(const std::string& name, TDescPtr type);
	virtual ~Field() {}					///< Destructor

	const std::string& name() const;	///< Return the field name
	TDescPtr type() const;				///< Return the field type

	/// Set my name...
	void name(const std::string& name) {
		_name = name;
	}
};

bool operator<(const Field& lhs, const Field& rhs);
bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& os, const Field& field);

/************************************************************************//**
 * A vector of Field's
 ****************************************************************************/
typedef std::vector<Field>	FieldVec;

/************************************************************************//**
 * Sub-Range - a contiguoush sub-range of integer values
 ****************************************************************************/
class SubRange {
	int		_min;						///< The sub-ranges minimum value
	int		_max;						///< The Sub-ranges maximum value

public:
	SubRange() : _min{0}, _max{0} {}	///< Default constructor
	SubRange(int minimum, int maximum);	///< Constructor
	virtual ~SubRange() {}				///< Destructor

	int minimum() const;				///< Return type sub-range minimum value
	int maximum() const;				///< Return type sub-range maximum value
	unsigned span() const;				///< Return type sub-range's span
};	

bool operator<(const SubRange& lhs, const SubRange& rhs);
bool operator==(const SubRange& lhs, const SubRange& rhs);
std::ostream& operator<<(std::ostream& os, const SubRange& srange);

/************************************************************************//**	
 * Type Descriptor a type classes size, sub-range and fields
 ****************************************************************************/
class TypeDesc {
public:
	/// Type kinds (type classes)
	enum Kind {
		Integer,						///< Whole number
		Real,							///< Real number
		Boolean,						///< Boolean value
		Character,						///< Character
		Array,							///< Array of some type
		Record,							///< Record of fields
		Enumeration,					///< Enumeration of constants
		Pointer							///< Pointer to a type
	};

	/**	Construct
	 * @param	size	Object size, in Datums
	 * @param	range	Sub-Range, defaults SubRange()
	 * @param	fields	Fields, defaults FieldVec()
	 */
	TypeDesc(	unsigned	size,
		const	SubRange&	range = SubRange(),
		const	FieldVec&	fields = FieldVec())
		: _size{size}, _range{range}, _fields{fields}
										{}

	virtual ~TypeDesc() {}				///< Destructor

	virtual Kind kind() const = 0;		///< Return my kind...

	/// Return my size, in Datums
	virtual unsigned size() const		{	return _size;					}

	/// Return my sub-range
	virtual const SubRange& range() const {	return _range;					}

	/** Return my array sub-rane (index) type. 
	 * @return TypeDesc()
	 */
	virtual TDescPtr rtype() const 		{	return TDescPtr();				}

	/** Return by base type
	 * @return TDescPtr()
	 */
	virtual TDescPtr base() const 		{	return TDescPtr();				}

	/// Return my fields
	virtual const FieldVec& fields() const	{	return _fields;				}

	/// Ordinal type?
	virtual bool isOrdinal() const 		{	return false;					}

protected:
	unsigned	_size;					///< Size, in Datums
	SubRange	_range;					///< My sub-range. For arrays, the array's span
	FieldVec	_fields;				///< My fields (enumeration and record)
};

std::ostream& operator<<(std::ostream& os, const TypeDesc::Kind& value);

bool operator<(const TypeDesc& lhs, const TypeDesc& rhs);
bool operator==(const TypeDesc& lhs, const TypeDesc& rhs);
std::ostream& operator<<(std::ostream& os, const TypeDesc& value);

#include <cassert>						/// TEMP!

/********************************************************************************************//**
 * Integer type descriptor
 * 
 * Integer and sub-ranges.
 ************************************************************************************************/
class IntDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	/// Construct an IntDesc from it's sub-range
	IntDesc(const SubRange& range) : Base(1, range) {}

	Kind kind() const override				{	return TypeDesc::Integer;			}
	bool isOrdinal() const override			{	return true;						}
};

/********************************************************************************************//**
 * Real type descriptor
 ************************************************************************************************/
class RealDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	RealDesc() : Base(1) {}					/// Construct a RealDesc
	Kind kind() const override				{	return TypeDesc::Real;				}
};

/********************************************************************************************//**
 * Boolean type descriptor
 ************************************************************************************************/
class BoolDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	BoolDesc() : Base(1) {}					/// Construct a BoolDesc

	Kind kind() const override				{	return TypeDesc::Boolean;			}
	bool isOrdinal() const override			{	return true;						}
};


/********************************************************************************************//**
 * Character type descriptor
 ************************************************************************************************/
class CharDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	/// Construct a CharDes from its sub-range
	CharDesc(const SubRange& range) : Base{1, range} {}

	Kind kind() const override				{	return TypeDesc::Character;			}
	bool isOrdinal() const override			{	return true;						}
};

/********************************************************************************************//**
 * Array type descriptor
 ************************************************************************************************/
class ArrayDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	/// Construct an ArrayDesc from it's size, index, type and base type
	ArrayDesc(unsigned size, const SubRange& index, TDescPtr rtype, TDescPtr base)
		: Base(size, index), _rtype{rtype}, _base{base} {}

	Kind kind() const override				{	return TypeDesc::Array;				}

	/** Return my size.
	 * @note Not sure why this override is needed
	 */
	virtual unsigned size() const override	{	return Base::size();				}

	/// Set, and then return, my size in Datums
	unsigned size(unsigned sz) 				{	return _size = sz;					}

	/// Return my range type
	TDescPtr rtype() const override 		{	return _rtype;						}

	/// Set, and then return, my range type
	TDescPtr rtype(TDescPtr type) 			{	return _rtype = type;				}

	/// Return my base type
	TDescPtr base() const override			{	return _base;						} 

	/// Set, and then return, my base type
	TDescPtr base(TDescPtr type) 			{	return _base = type;				}

	bool isOrdinal() const override			{	return false;						}

private:
	TDescPtr	_rtype;						///< Arrays index (sub-range) type
	TDescPtr	_base;						///< Arrays base type
};

/********************************************************************************************//**
 * Record type descriptor
 ************************************************************************************************/
class RecordDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	/// Construct a RecordDesc from it's size ,and fields
	RecordDesc(unsigned size, const FieldVec& fields)
		: Base(size, SubRange(), fields) {}

	Kind kind() const override				{	return TypeDesc::Record;			}
};
 
/********************************************************************************************//**
 * Enumeration type descptor
 ************************************************************************************************/
class EnumDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	/// Construct a EnumDesc from it's range and fields
	EnumDesc(const SubRange& range, const FieldVec& fields)
		: Base(1, range, fields) 			{}

	Kind kind() const override				{	return TypeDesc::Enumeration;		}

	/** Return my fields
	 * @note Don't know why this is needed
	 */
	const FieldVec& fields() const override	{	return Base::fields();				}

	/// Set, and return, my fields
	const FieldVec& fields(const FieldVec& fields) {
		return _fields = fields;
	}

	bool isOrdinal() const override			{	return true;						}
};

/********************************************************************************************//**
 * Pointer type description
 ************************************************************************************************/
class PtrDesc : public TypeDesc {
public:
	typedef	TypeDesc		Base;			///< My base type

	/// Construct an PtrDesc from base type
	PtrDesc(TDescPtr base) : Base(1), _base{base} {}

	Kind kind() const override				{	return TypeDesc::Pointer;			}

	/// Return my base type
	TDescPtr base() const override			{	return _base;						} 

	bool isOrdinal() const override			{	return false;						}

private:
	TDescPtr	_base;						///< Arrays base type
};

/********************************************************************************************//**
 * Type Descriptor
 *
 * Describes ordinals (integers, booleans, characters, enumerations),
 * sub-ranges of ordinals, reals structured types (arrays and records).
 *
 * A number of pre-defined types are provided; integers, constant integers,
 * real, character and boolean.
 ************************************************************************************************/
class TDesc {
public:
	/// Type kinds (type classe 
	typedef TypeDesc::Kind	Kind;

	// useful constants

	static SubRange	maxRange;			///< Largest possible range

	// pre-defined types

	static TDescPtr intDesc;			///< Integer type description
	static TDescPtr constIntDesc;		///< Constant integer type description
	static TDescPtr realDesc;			///< Real type description
	static TDescPtr constRealDesc;		///< Constant real type description
	static TDescPtr charDesc;			///< Character type description
	static TDescPtr boolDesc;			///< Boolean type descriptor

	/// Create, and return, a TDescPtr to a new IntDesc
	static TDescPtr newIntDesc(const SubRange& range = SubRange());

	static TDescPtr newRealDesc();		///< Create, and return, a TDescPtr to a new RealDesc
	static TDescPtr newBoolDesc();		///< Create, and return, a TDescPtr to a BoolDesc

	/// Create, and return, a TDescPtr to a new CharDesc
	static TDescPtr newCharDesc(const SubRange& range);

	/// Create, and return, a TDescPtr to a new ArrayDesc
	static TDescPtr newArrayDesc(
				unsigned	size,
		const	SubRange&	range,
				TDescPtr	rtype,
				TDescPtr	base = TDescPtr());

	/// Create, and return, a TDescPtr to a new RecordDesc
	static TDescPtr newRcrdDesc(unsigned size, const FieldVec& fields = FieldVec());

	/// Create, and return, a TDescPtr to a new EnumDesc
	static TDescPtr newEnumDesc(const SubRange& range, const FieldVec& fields = FieldVec());

	/// Create, and return, a TDescPtr to a new PtrDesc
	static TDescPtr newPtrDesc(TDescPtr base);

	virtual ~TDesc() {}					///< Destructor
};

#endif

