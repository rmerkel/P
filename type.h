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

class TDesc; 

/************************************************************************//**	
 * Pointer to TDesc
 ****************************************************************************/
typedef std::shared_ptr<TDesc>	TDescPtr;

/************************************************************************//**	
 * Vector of TDescPtrs
 ****************************************************************************/
typedef std::vector<TDescPtr>	TDescPtrVec;

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
	size_t span() const;				///< Return type sub-range's span
};	

bool operator<(const SubRange& lhs, const SubRange& rhs);
bool operator==(const SubRange& lhs, const SubRange& rhs);
std::ostream& operator<<(std::ostream& os, const SubRange& srange);

/************************************************************************//**	
 * Type Descriptor
 *
 * Describes ordinals (integers, booleans, characters, enumerations),
 * sub-ranges of ordinals, reals structured types (arrays and records).
 *
 * A number of pre-defined types are provided; integers, constant integers,
 * real, character and boolean.
 ****************************************************************************/
class TDesc {
public:
	/// Type kinds (type classes)
	enum Kind {
		Integer,				///< Whole number
		Real,					///< Real number
		Boolean,				///< Boolean value
		Character,				///< Character
		Array,					///< Array of some type
		Record,					///< Record of fields
		Enumeration				///< Enumeration of constants
	};

	// useful constants

	static SubRange	maxRange;			///< Largest possible range

	// pre-defined types

	static TDescPtr intDesc;			///< Integer type description
	static TDescPtr constIntDesc;		///< Constant integer type description
	static TDescPtr realDesc;			///< Real type description
	static TDescPtr constRealDesc;		///< Constant real type description
	static TDescPtr charDesc;			///< Character type description
	static TDescPtr boolDesc;			///< Boolean type descriptor

	/// Create and return a shared pointer to a new TDesc
	static TDescPtr newTDesc(
				Kind		kind,
				unsigned	size,
		const	SubRange&	range		= SubRange(),
				TDescPtr	rtype		= TDescPtr(),
				TDescPtr	base		= TDescPtr(),
		const	FieldVec&	fields		= FieldVec());

	/// Constructor
	TDesc(		Kind		kind,
				unsigned	size,
		const	SubRange&	range		= SubRange(),
				TDescPtr	rtype		= TDescPtr(),
				TDescPtr	base		= TDescPtr(),
		const	FieldVec&	fields		= FieldVec());

	virtual ~TDesc() {}					///< Destructor

	Kind kind() const;					///< Return my kind...
	Kind kind(Kind kind);				///< Set, and then, return my kind

	unsigned size() const;				///< Return my size, in Datums
	unsigned size(unsigned sz);			///< Set, and return my size, in Datums

	const SubRange& range() const;		///< Return my sub-range

	/// Set and return my sub-range
	const SubRange& range(const SubRange& range);

	TDescPtr base() const;				///< Return by base type
	TDescPtr base(TDescPtr type);		///< Set, and return by base type

	TDescPtr rtype() const;				///< Return my array index type
	TDescPtr rtype(TDescPtr type);		///< Set, and return my array index type

	const FieldVec& fields() const;		///< Return my fields

	/// Set and return my fields
	const FieldVec& fields(const FieldVec& flds);

	bool isOrdinal() const;				///< Ordinal type?

private:
	Kind		_kind;					///< My kind, (type class)
	unsigned	_size;					///< Size of on object of my type, in Datums
	SubRange	_range;					///< My sub-range. For arrays, the array span
	TDescPtr	_rtype;					///< Arrays index (sub-range) type
	TDescPtr	_base;					///< Base type for Elementry, Array, Enumeration
	FieldVec	_fields;				///< List of fields for Record
};

bool operator<(const Field& lhs, const Field& rhs);
bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& os, const TDesc::Kind& value);
std::ostream& operator<<(std::ostream& os, const TDesc& value);

#endif

