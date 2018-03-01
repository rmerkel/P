/********************************************************************************************//**
 * @file type.h
 *
 * The P language type system. Type descriptors, record fields and sub-ranges.
 ************************************************************************************************/

#ifndef	TYPE_H
#define	TYPE_H

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>

class TypeDesc; 

/********************************************************************************************//**
 * Pointer to TypeDesc
 ************************************************************************************************/
typedef std::shared_ptr<TypeDesc>		TDescPtr;

/********************************************************************************************//**
 * Vector of TDescPtrs
 ************************************************************************************************/
typedef std::vector<TDescPtr>			TDescPtrVec;

/********************************************************************************************//**
 * Type Field - record field name and type pairs
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
 * Sub-Range - a contiguoush sub-range of integer values
 ************************************************************************************************/
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

/********************************************************************************************//**
 * Type Descriptor a type classes size, sub-range and fields
 *
 * Describes both built-in/pre-defined types and user defined types with in
 * the following type kinds (classes):
 *
 * | Kind         | Size | Sub-Range   | IType | Base | Fields | Ordinal |
 * | :----------- | :--: | :---------: | :---: | :--: | :----: | :-----: |
 * |  Array       |	N    |   1 - 10    |  T1   |  T2  |   -    |	N    |
 * |  Boolean     |	1    |   0 - 1     |   -   |   -  |   -    |	Y    |
 * |  Character   |	1    |   0 - 127   |   -   |   -  |   -    |	Y    |
 * |  Enumeration |	1    |   X - Y     |   -   |   -  |   -    |	Y    |
 * |  Integer	  |	1    | IMIN - IMAX |   -   |   -  |   -    |	Y    |
 * |  Pointer     |  1   |     -       |   -   |   T  |   -    |    N    |
 * |  Real        |	1    |     -       |   -   |   -  |   -    |	N    |
 * |  Record      |	N    |     -       |   -   |   -  | Fields |    N    |
 * |  Set         |	N    |     ?       |   -   |  T2  | Fields |    ?    |
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
		Record,
		Set
	};

	// useful constants

	static SubRange	maxRange;			///< Largest possible range

	// pre-defined types

	static TDescPtr boolDesc;			///< Boolean type descriptor
	static TDescPtr charDesc;			///< Character type description
	static TDescPtr intDesc;			///< Integer type description
	static TDescPtr constIntDesc;		///< Constant integer type description
	static TDescPtr realDesc;			///< Real type description
	static TDescPtr constRealDesc;		///< Constant real type description

	/// Create, and return, a TDescPtr to a new IntDesc
	static TDescPtr newIntDesc(const SubRange& range = SubRange());

	static TDescPtr newRealDesc();		///< Create, and return, a TDescPtr to a new RealDesc
	static TDescPtr newBoolDesc();		///< Create, and return, a TDescPtr to a BoolDesc

	/// Create, and return, a TDescPtr to a new CharDesc
	static TDescPtr newCharDesc(const SubRange& range);

	/// Create, and return, a TDescPtr to a new ArrayDesc
	static TDescPtr newArrayDesc(
				size_t		size,
		const	SubRange&	range,
				TDescPtr	rtype,
				TDescPtr	base = TDescPtr());

	/// Create, and return, a TDescPtr to a new RecordDesc
	static TDescPtr newRcrdDesc(size_t size, const FieldVec& fields = FieldVec());

	/// Create, and return, a TDescPtr to a new EnumDesc
	static TDescPtr newEnumDesc(const SubRange& range, const FieldVec& fields = FieldVec());

	/// Create, and return, a TDescPtr to a new PointerDesc
	static TDescPtr newPointerDesc(TDescPtr base);

	virtual ~TypeDesc() {}				///< Destructor

	TypeClass tclass() const;			///< Return my type class
	size_t size() const;				///< Return my size, in Datums
	size_t size(size_t sz); 			///< Set, and then return, my size in Datums
	const SubRange& range() const;		///< Return my sub-range
	TDescPtr itype() const;				///< Return my array sub-rane (index) type. 
	TDescPtr itype(TDescPtr type);		///< Set, and then return, my range type
	const FieldVec& fields() const;		///< Return my fields

	/// Return my fields
	const FieldVec& fields(const FieldVec& fields);

	TDescPtr base() const;				///< Return by base type
	TDescPtr base(TDescPtr type);		///< Set, and then return, my base type
	bool isOrdinal() const;				///< Return true if I'm an ordinal

protected:
	/**	Construct
	 * @param	tclass	My type class
	 * @param	size	Object size, in Datums
	 * @param	range	Sub-Range, defaults SubRange()
	 * @param	fields	Fields, defaults FieldVec()
	 */
	TypeDesc(	TypeClass	tclass,
				size_t		size,
		const	SubRange&	range	= SubRange(),
				TDescPtr	itype	= TDescPtr(),
		const	FieldVec&	fields	= FieldVec(),
				TDescPtr	base	= TDescPtr(),
				bool		ordinal = false);

private:
	TypeClass	_tclass;					///< Type class
	size_t		_size;						///< Size, in Datums
	SubRange	_range;						///< My sub-range. For arrays, the array's span
	TDescPtr	_itype;						///< Arrays index (sub-range) type
	FieldVec	_fields;					///< My fields (enumeration and record)
	TDescPtr	_base;						///< Arrays base type
	bool		_ordinal;					///< An ordinal?

};

bool operator<(const TypeDesc& lhs, const TypeDesc& rhs);
bool operator==(const TypeDesc& lhs, const TypeDesc& rhs);
std::ostream& operator<<(std::ostream& os, TypeDesc::TypeClass);
std::ostream& operator<<(std::ostream& os, const TypeDesc& value);

#endif
