/************************************************************************//**
 * @file type.h
 *
 * Pascal-Lite type system. Type descriptors, record fields and sub-ranges.
 ****************************************************************************/

#ifndef	TYPE_H
#define	TYPE_H

#include <string>
#include <memory>
#include <vector>

/************************************************************************//**
 * Type Field - record field name and type pairs
 ****************************************************************************/
class Field {
	std::string	_name;					///< Name of the field
	std::string	_type;					///< Field's type, e.g., "integer", "X"

public:
	Field() {}							///< Default constructor
										/// Constructor
	Field(const std::string& name, const std::string& type);
	virtual ~Field() {}				///< Destructor

										/// Return the field's name
	const std::string& name() const		{	return _name;		}

										/// Return the field's type
	const std::string& type() const		{	return _type;		}
};

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
 * Vector of shared pointers to TDescs
 ****************************************************************************/
typedef std::vector<TDescPtr>	TDescPtrVec;

/************************************************************************//**	
 * Type Descriptor
 ****************************************************************************/
class TDesc {
public:
	/// Type class kinds
	enum Kind {	Integer, Real, Boolean, Character, Array, SRange, Record, Enumeration	};

	/// Return kind as a stirng
	static const std::string toString(Kind kind);

	/// Create and return a shared pointer to a new TDesc
	static TDescPtr newTDesc(
		Kind				kind,
		unsigned			size,
		const SubRange&		range,
		const std::string&	base,
		const FieldVec&		fields);

	virtual ~TDesc() {}					///< Destructor

	/// Return my kind
	Kind kind() const					{	return _kind;		}

	/// Return my size, in bytes
	unsigned size() const				{	return _size;		}

	/// Return my sub-range
	const SubRange& range() const		{	return _range;		}

	/// Return my base type
	const std::string& base() const		{	return _base;		}

	/// Return my fields
	const FieldVec& fields() const		{	return _fields;		}

protected:
	/// Constructor
	TDesc(	Kind				kind,
			unsigned			size,
			const SubRange&		range,
			const std::string&	base,
			const FieldVec&		fields);

private:
	Kind		_kind;					///< My kind
	unsigned	_size;					///< Size of on object of my type
	SubRange	_range;					///< My sub-range
	std::string	_base;					///< Base type for Elementry, Array, Enumeration
	FieldVec	_fields;				///< List of fields for Record
};

#endif

