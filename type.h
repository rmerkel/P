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

class iTDesc; 

/************************************************************************//**	
 * Pointer to TDesc
 ****************************************************************************/
typedef std::shared_ptr<iTDesc>	TDescPtr;

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
 * Type Descriptor interface
 ****************************************************************************/
class iTDesc {
public:
	/// Type kinds (type classes)
	enum Kind {
		Integer,						///< Whole number
		Real,							///< Real number
		Boolean,						///< Boolean value
		Character,						///< Character
		Array,							///< Array of some type
		Record,							///< Record of fields
		Enumeration						///< Enumeration of constants
	};

	virtual ~iTDesc() {}				///< Destructor

	virtual Kind kind() const = 0;		///< Return my kind...
	virtual Kind kind(Kind kind) = 0;	///< Set, and then, return my kind

	virtual unsigned size() const = 0;	///< Return my size, in Datums

	/// Set, and return my size, in Datums
	virtual unsigned size(unsigned sz) = 0;

	/// Return my sub-range
	virtual const SubRange& range() const = 0;

	/// Set and return my sub-range
	virtual const SubRange& range(const SubRange& range) = 0;

	virtual TDescPtr rtype() const = 0;	///< Return my array index type

	/// Set, and return my array index type
	virtual TDescPtr rtype(TDescPtr type) = 0;

	virtual TDescPtr base() const = 0;	///< Return by base type

	/// Set, and return by base type
	virtual TDescPtr base(TDescPtr type) = 0;

	/// Return my fields
	virtual const FieldVec& fields() const = 0;

	/// Set and return my fields
	virtual const FieldVec& fields(const FieldVec& flds) = 0;

	/// Ordinal type?
	virtual bool isOrdinal() const = 0;
};

bool operator<(const iTDesc& lhs, const iTDesc& rhs);
bool operator==(const iTDesc& lhs, const iTDesc& rhs);
std::ostream& operator<<(std::ostream& os, const iTDesc::Kind& value);
std::ostream& operator<<(std::ostream& os, const iTDesc& value);

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
	/// Type kinds (type classe 
	typedef iTDesc::Kind	Kind;

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

	virtual ~TDesc() {}					///< Destructor
};

#endif

