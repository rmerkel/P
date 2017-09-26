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
 * Shared pointer to TDesc
 ****************************************************************************/
typedef std::shared_ptr<TDesc>	TDescPtr;

/************************************************************************//**	
 * constant TDescPtr.
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
										/// Constructor
	Field(const std::string& name, ConstTDescPtr type);
	virtual ~Field() {}					///< Destructor

										/// Return the field's name
	const std::string& name() const		{	return _name;		}

										/// Return the field's type
	ConstTDescPtr type() const			{	return _type;		}
};

/************************************************************************//**
 * @brief Field put operator
 *
 * @param	os		Stream to put the value one
 * @param	field	Value to put on os
 ****************************************************************************/
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

	/// Return minimum value
	int minimum() const					{	return _min;		}

	/// Return maximum value
	int maximum() const					{	return _max;		}

	/// Return my span
	size_t span() const;
};	

/************************************************************************//**
 * @brief SubRange put operator
 *
 * @param	os		Stream to put the value one
 * @param	srange	Value to put on os
 ****************************************************************************/
std::ostream& operator<<(std::ostream& os, const SubRange& srange);

/************************************************************************//**	
 * Type Descriptor
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
		const	SubRange&		range = SubRange(),
				ConstTDescPtr	base = ConstTDescPtr(),
		const	FieldVec&		fields = FieldVec());

	/// Default constructor
	TDesc() : _kind(Integer), _size(0)	{}

	/// Constructor
	TDesc(			Kind			kind,
					unsigned		size,
			const	SubRange&		range	= SubRange(),
					ConstTDescPtr	base	= ConstTDescPtr(),
			const	FieldVec&		fields	= FieldVec());

	virtual ~TDesc() {}					///< Destructor

	/// Return my kind
	Kind kind() const					{	return _kind;		}

	/// Return my size, in bytes
	unsigned size() const				{	return _size;		}

	/// Return my sub-range
	const SubRange& range() const		{	return _range;		}

	/// Return my base type
	ConstTDescPtr base() const			{	return _base;		}

	/// Return my fields
	const FieldVec& fields() const		{	return _fields;		}

	bool isOrdinal() const;				///< Ordinal type?
	bool isElmentary() const;			///< Elementary type? 

private:
	Kind			_kind;				///< My kind
	unsigned		_size;				///< Size of on object of my type
	SubRange		_range;				///< My sub-range
	ConstTDescPtr	_base;				///< Base type for Elementry, Array, Enumeration
	FieldVec		_fields;			///< List of fields for Record
};

std::ostream& operator<<(std::ostream& os, const TDesc::Kind& value);
std::ostream& operator<<(std::ostream& os, const TDesc& value);

#endif

