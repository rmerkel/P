/********************************************************************************************//**
 * @file datum.h
 *
 * The P Machine's data type.
 *
 * class Datum.
 ************************************************************************************************/

#ifndef	DATUM_H
#define DATUM_H

#include <iostream>
#include <utility>
#include <vector>

/********************************************************************************************//**
 * A Data Value
 *  
 * Datums contain either a boolean, character, signed interger, floating point value, or an
 * address of another Datum (pointer to another Datum). Converstion between types is limited.
 *  
 * A discriminator (kind()), which is initialized by the constructors, and enforced by the
 * operators, which throw a Datum::Error, i.e., bitwise operators on Real values are illegal.
 ************************************************************************************************/
class Datum {
public:
	/// Datum "kinds"
	enum Kind {
		Boolean,							///< Boolean value
		Character,							///< Character (ASCII) value
		Integer,							///< Signed integer
		Real,								///< Floating point
		Address								///< Unsigned, or a reference to another Datum
	};

	/// Thrown on runtime errors...
	class Error : public std::runtime_error {
	public: explicit Error(const char* what);
	};

 	/// Thrown if an illegal operation, for the given parameters, attempted
	class IllegalOp : public Datum::Error {
		public: explicit IllegalOp(const char* what);
	};

 	///  Throw if attempt to divide by zero attempted
	class DivideByZero : public Datum::Error {
		public: explicit DivideByZero();
	};

	Datum();								///< Default constructor...
	Datum(const Datum& datum);				///< Copy constructor
	Datum(Datum&& datum);					///< Move constructor

	explicit Datum(bool value);				///< Construct a Boolean...
	explicit Datum(char value);				///< Construct a Character...
	explicit Datum(int value);				///< Construct a Integer...
	explicit Datum(double value);			///< Construct a Double...
	explicit Datum(std::size_t value);		///< Construct an Address...

	virtual ~Datum() {}						///< Destructor

	Datum& operator=(const Datum& value);	///< Assignment...
	Datum& operator=(Datum&& value);		///< Move assignment...

	Datum& operator=(bool value);			///< Assignment (Boolean)...
	Datum& operator=(char value);			///< Assignment (Character)...
	Datum& operator=(int value);			///< Assignment (Integer)...
	Datum& operator=(double value);			///< Assignment (Real)...
	Datum& operator=(std::size_t value);	///< Assignment (Address)...

	Datum operator!() const;				///< Unary boolean negation...
	Datum operator-() const;				///< Unary negation...
	Datum operator~() const;				///< Unary bitwise not...

	Datum& operator++();					///< Prefix increment
	Datum& operator--();					///< Prefix decrement
	Datum operator++(int);					///< Postfix increment
	Datum operator--(int);					///< Postfix decrement

	Datum& operator+=(const Datum& rhs);	///< Subtract...
	Datum& operator-=(const Datum& rhs);	///< Subtract...
	Datum& operator*=(const Datum& rhs);	///< Multiplication...
	Datum& operator/=(const Datum& rhs);	///< Division...
	Datum& operator%=(const Datum& rhs);	///< Remander...

	Datum& operator&=(const Datum& rhs);	///< Bit-wise And...
	Datum& operator|=(const Datum& rhs);	///< Bit-wise Or...
	Datum& operator^=(const Datum& rhs);	///< Bit-wise Exclusive-Or...

	Kind kind() const;						///< Return my kind...

	bool boolean() const;					///< Return my Boolean value...
	char character() const;					///< Return my Character...
	int integer() const;					///< Return my Integer value...
	unsigned natural() const;				///< Return my Natual value...
	double real() const;					///< Return my Real value...
	size_t address() const;					///< Return my Address value...

	bool numeric() const;					///< Return true if value is numeric...
	bool ordinal() const;					///< Return true if value is ordinal...
	bool zero() const;						///< Return true if value is equal to zero...

private:
	union {
		bool		b;						///< if k == Boolean
		char		c;						///< if k == Character
		int			i;						///< if k == Integer
		double		r;						///< if k == Real
		size_t		a;						///< if k == Address
	};
	Kind			k;  					///< What Datum type?
};

/********************************************************************************************//**
 * A vector of Datums
 ************************************************************************************************/
typedef	std::vector<Datum>	DatumVector;

std::ostream& operator<<(std::ostream& os, const Datum::Kind& kind);
std::ostream& operator<<(std::ostream& os, const Datum& value);

Datum operator+(const Datum& lhs, const Datum& rhs);
Datum operator-(const Datum& lhs, const Datum& rhs);
Datum operator*(const Datum& lhs, const Datum& rhs);
Datum operator/(const Datum& lhs, const Datum& rhs);
Datum operator%(const Datum& lhs, const Datum& rhs);

Datum operator&(const Datum& lhs, const Datum& rhs);
Datum operator|(const Datum& lhs, const Datum& rhs);
Datum operator^(const Datum& lhs, const Datum& rhs);

Datum operator<<(const Datum& lhs, const Datum& rhs);
Datum operator>>(const Datum& lhs, const Datum& rhs);

bool operator <(const Datum& lhs, const Datum& rhs);
bool operator==(const Datum& lhs, const Datum& rhs);

bool operator&&(const Datum& lsh, const Datum& rhs);
bool operator||(const Datum& lsh, const Datum& rhs);

#endif
