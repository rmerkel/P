/********************************************************************************************//**
 * @file type.h
 *
 * The P language type system. Type descriptors, record fields and sub-ranges.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	TYPE_H
#define	TYPE_H

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>

#include "subrange.h"

class TypeDesc; 

/********************************************************************************************//**
 * Pointer to a TypeDesc
 ************************************************************************************************/
typedef std::shared_ptr<TypeDesc>		TDescPtr;

/********************************************************************************************//**
 * Pointer to a const TypeDesc
 ************************************************************************************************/
typedef std::shared_ptr<const TypeDesc>	ConstTDescPtr;

/********************************************************************************************//**
 * Vector of TDescPtr
 ************************************************************************************************/
typedef std::vector<TDescPtr>			TDescPtrVec;

/********************************************************************************************//**
 * Type Field - record field name and type pair
 ************************************************************************************************/
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

/********************************************************************************************//**
 * A vector of Field's
 ************************************************************************************************/
typedef std::vector<Field>	FieldVec;

/********************************************************************************************//**
 * Type Descriptor a type classes size, sub-range and fields
 *
 * Describes both built-in/pre-defined types and user defined types with in
 * the following type kinds (classes):
 *
 * | Kind       |Size|    Subrange    |IType|Base|Fields|Ordinal|
 * |:-----------|:--:|:--------------:|:---:|:--:|:----:|:-----:|
 * | Array      | N  |      1..10     | T1  | T2 |  -   |   N   |
 * | Boolean    | 1  |      0..1      |  -  |  - |  -   |   Y   |
 * | Character  | 1  |      0..127    |  -  |  - |  -   |   Y   |
 * | Enumeration| 1  |      X..Y      |  -  |  - |  -   |   Y   |
 * | Integer	| 1  |INT_MIN..INT_MAX|  -  |  - |  -   |   Y   |
 * | Pointer    | 1  |       -        |  -  |  T |  -   |   N   |
 * | Real       | 1  |       -        |  -  |  - |  -   |   N   |
 * | Record     | N  |       -        |  -  |  - |Fields|   N   |
 *
 * Key:
 * - IType - is the sub-range (index) type for arrays
 * - Base - is the base type for arrays, sub-ranges and enumerations
 * - Fields - is a list of name/type pairs to identify record fields
 ************************************************************************************************/
class TypeDesc {
public:
	/// The type class
	enum TypeClass {
		Array,
		Boolean,
		Character,
		Enumeration,
		Integer,
		Pointer,
		Real,
		Record
	};

	// useful constants

	static const Subrange charRange;		///< Range of ASCII characters
	static const Subrange maxRange;			///< Largest possible range

	/// Create, and return, a TDescPtr to a new IntDesc
	static TDescPtr newIntDesc(const Subrange& range = maxRange, bool ref = false);

	/// Create, and return, a TDescPtr to a new RealDesc
	static TDescPtr newRealDesc(bool ref = false);

	/// Create, and return, a TDescPtr to a BoolDesc
	static TDescPtr newBoolDesc(bool ref = false);

	/// Create, and return, a TDescPtr to a new CharDesc
	static TDescPtr newCharDesc(const Subrange& = charRange, bool ref = false);

	/// Create, and return, a TDescPtr to a new ArrayDesc
	static TDescPtr newArrayDesc(
				size_t		size,
		const	Subrange&	range,
				TDescPtr	rtype,
				TDescPtr	base = TDescPtr(),
				bool		ref = false);

	/// Create, and return, a TDescPtr to a new RecordDesc
	static TDescPtr newRcrdDesc(size_t size, const FieldVec& fields = FieldVec(), bool ref = false);

	/// Create, and return, a TDescPtr to a new EnumDesc
	static TDescPtr newEnumDesc(const Subrange& range, const FieldVec& fields = FieldVec(), bool ref = false);

	/// Create, and return, a TDescPtr to a new PointerDesc
	static TDescPtr newPointerDesc(TDescPtr base, bool ref = false);

	/// Return a new copy of a TypeDesc
	static TDescPtr clone(TDescPtr tdesc);

	virtual ~TypeDesc() {}				///< Destructor

	TypeClass tclass() const;			///< Return my type class
	size_t size() const;				///< Return my size, in Datums
	size_t size(size_t sz); 			///< Set, and then return, my size in Datums
	const Subrange& range() const;		///< Return my sub-range
	TDescPtr itype() const;				///< Return my array sub-rane (index) type. 
	TDescPtr itype(TDescPtr type);		///< Set, and then return, my range type
	const FieldVec& fields() const;		///< Return my fields

	/// Return my fields
	const FieldVec& fields(const FieldVec& fields);

	TDescPtr base() const;				///< Return by base type
	TDescPtr base(TDescPtr type);		///< Set, and then return, my base type
	bool ordinal() const;				///< Return true if I'm an ordinal
	bool ref(bool v);					///< Set, and then return, if this type is passed by reference
	bool ref() const;					///< Return true if this type is passed by reference

protected:
	///	Constructor
	TypeDesc(	TypeClass	tclass,
				size_t		size,
		const	Subrange&	range	= Subrange(),
				TDescPtr	itype	= TDescPtr(),
		const	FieldVec&	fields	= FieldVec(),
				TDescPtr	base	= TDescPtr(),
				bool		ordinal = false,
				bool		ref		= false);

private:
	TypeClass	_tclass;					///< Type class
	size_t		_size;						///< Size, in Datums
	Subrange	_range;						///< My sub-range. For arrays, the array's span
	TDescPtr	_itype;						///< Arrays index (sub-range) type
	FieldVec	_fields;					///< My fields (enumeration and record)
	TDescPtr	_base;						///< Arrays base type
	bool		_ordinal;					///< An ordinal?
	bool		_ref;						///< passed by reference?
};

bool operator<(const TypeDesc& lhs, const TypeDesc& rhs);
bool operator==(const TypeDesc& lhs, const TypeDesc& rhs);
std::ostream& operator<<(std::ostream& os, TypeDesc::TypeClass);
std::ostream& operator<<(std::ostream& os, const TypeDesc& value);

#endif

