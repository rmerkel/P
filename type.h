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
	std::string	_name;					///< Name of the field
	TDescPtr	_type;					///< Fields type

public:
	Field() {}							///< Default constructor
	Field(const std::string& name, TDescPtr type);
	virtual ~Field() {}					///< Destructor

	const std::string& name() const;	///< Return the fields name
	TDescPtr type() const;				///< Return the fields type
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
	int		_min;						///< Minimum value
	int		_max;						///< Maximum value

public:
	SubRange() : _min{0}, _max{0} {}	///< Default constructor
	SubRange(int minimum, int maximum);	///< Constructor
	virtual ~SubRange() {}				///< Destructor

	int minimum() const;				///< Return my minimum value
	int maximum() const;				///< Return my maximum value
	size_t span() const;				///< Return my range's span
};	

bool operator<(const Field& lhs, const Field& rhs);
bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& os, const SubRange& srange);

/************************************************************************//**	
 * Type Descriptor
 *
 * Describes both built-in/pre-defined types and user defined types with in
 * the following type kinds (classes):
 *
 * | Kind    | Size | Sub-Range | IType | Base | Fields | Ordinal | Elementry
 * | :------ | :--: | :-------: | :---: | :--: | :----: | :-----: | :-------:
 * |  Int	 |	1   | IMIN-IMAX |   -   |   -  |   -    |	Y     |	   Y     
 * |  Real   |	1   |     -     |   -   |   -  |   -    |	N     |	   Y     
 * |  Bool   |	1   |   0 - 1   |   -   |   -  |   -    |	Y     |    Y     
 * |  Char   |	1   |   0 - 127 |   -   |   -  |   -    |	Y     |	   Y     
 * |  Array  |	N   |    1-10   |  T1   |  T2  |   -    |	N     |	   N     
 * |  Int    |	1   |    1-10   |   -   |   -  |   -    |	Y     |	   N	 
 * |  Record |	N   |     -     |   -   |   -  | Fields |   N     |	   N     
 * |  Enum	 |	1   |    X-Y    |   -   |   -  |   -    |	Y     |	   N     
 *
 * Key:
 * - IType - is the sub-range (index) type for arrays
 * - Base - is the base type for arrays, sub-ranges and enumerations
 * - Fields - is a list of name/type pairs to identify record fields
 ****************************************************************************/
class TDesc {
public:
	/// Type class kinds
	enum Kind {
		Integer,				///< Whole number
		Real,					///< Real number
		Boolean,				///< Boolean value
		Character,				///< Character
		Array,					///< Array of some type
		Record,					///< Record of fields
		Enumeration				///< Enumeration of constants
	};

	/// Create and return a shared pointer to a new TDesc
	static TDescPtr newTDesc(
				Kind		kind,
				unsigned	size,
		const	SubRange&	range		= SubRange(),
				TDescPtr	rtype		= TDescPtr(),
				TDescPtr	base		= TDescPtr(),
		const	FieldVec&	fields		= FieldVec());

	// pre-defined types

	static TDescPtr intDesc;			///< Integer type description
	static TDescPtr constIntDesc;		///< Constant integer type description
	static TDescPtr realDesc;			///< Real type description
	static TDescPtr constRealDesc;		///< Constant real type description
	static TDescPtr charDesc;			///< Character type description
	static TDescPtr boolDesc;			///< Boolean type descriptor

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
	bool isElmentary() const;			///< Elementary type? 

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

