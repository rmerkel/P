/************************************************************************//**
 * @file type.h
 *
 * Pascal-Lite type system. Type descriptors, record fields and sub-ranges.
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
 * Constant TDescPtr.
 ****************************************************************************/
typedef const TDescPtr			ConstTDescPtr;

/************************************************************************//**	
 * Vector of TDescPtrs
 ****************************************************************************/
typedef std::vector<TDescPtr>	TDescPtrVec;

/************************************************************************//**
 * Type Field - record field name and type pairs
 ****************************************************************************/
class Field {
	std::string		_name;				///< Name of the field
	ConstTDescPtr	_type;				///< Fields type

public:
	Field() {}							///< Default constructor
	Field(const std::string& name, ConstTDescPtr type);
	virtual ~Field() {}					///< Destructor

	const std::string& name() const;	///< Return the fields name
	ConstTDescPtr type() const;			///< Return the fields type
};

bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& os, const Field& field);

/************************************************************************//**
 * A vector of Field's
 ****************************************************************************/
typedef std::vector<Field>	FieldVec;

/************************************************************************//**
 * Sub-Range - minimum to maximum
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

bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& os, const SubRange& srange);

/************************************************************************//**	
 * Type Descriptor
 *                       Index
 * Kind   size Sub-Range Type Base   Fields Ordinal Elementry
 * ------ ---- --------- ---- ------ ------ ------- ---------
 * None     0	   - 	  -		-		-	   N	   N
 * Int		1  IMIN-IMAX  -		-		-	   Y	   Y
 * Real     1      -      -		-		-	   N	   Y
 * Bool     1    0 - 1    -		-		-	   Y       Y
 * Char     1    0 - 127  -		-		-	   Y	   Y
 * Array    N   Min-Max   T1	T2		-	   N	   N
 * SRange   1	Min-Max   -		Int		-	   Y	   N
 * Record   N	   -	  -		-	  Fields   N	   N
 * Enum		1	Min-Max   -		Int		-	   Y	   N
 ****************************************************************************/
class TDesc {
public:
	/// Type class kinds
	enum Kind {
		None,						/// Placeholder for a valid type
		Integer,
		Real,
		Boolean,
		Character,
		Array,
		SRange,
		Record,
		Enumeration
	};

	/// Create and return a shared pointer to a new TDesc
	static TDescPtr newTDesc(
				Kind			kind,
				unsigned		size,
		const	SubRange&		range	= SubRange(),
				ConstTDescPtr	rtype	= ConstTDescPtr(),
				ConstTDescPtr	base	= ConstTDescPtr(),
		const	FieldVec&		fields	= FieldVec());

	TDesc() : _kind(None), _size(0)	{}	///< Default constructor

	/// Constructor
	TDesc(		Kind			kind,
				unsigned		size,
		const	SubRange&		range	= SubRange(),
				ConstTDescPtr	rtype	= ConstTDescPtr(),
				ConstTDescPtr	base	= ConstTDescPtr(),
		const	FieldVec&		fields	= FieldVec());

	virtual ~TDesc() {}					///< Destructor

	Kind kind() const;					///< Return my kind...
	unsigned size() const;				///< Return my size, in Datums
	const SubRange& range() const;		///< Return my sub-range
	ConstTDescPtr rtype() const;		///< Return my array index type
	ConstTDescPtr base() const;			///< Return by base type
	const FieldVec& fields() const;		///< Return my fields
	bool isOrdinal() const;				///< Ordinal type?
	bool isElmentary() const;			///< Elementary type? 

private:
	Kind			_kind;				///< My kind
	unsigned		_size;				///< Size of on object of my type
	SubRange		_range;				///< My sub-range
	ConstTDescPtr	_rtype;				///< Arrays index (sub-range) type
	ConstTDescPtr	_base;				///< Base type for Elementry, Array, Enumeration
	FieldVec		_fields;			///< List of fields for Record
};

bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& os, const TDesc::Kind& value);
std::ostream& operator<<(std::ostream& os, const TDesc& value);

#endif

