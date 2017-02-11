/**	@file	token.h
 *
 *	@brief	enum Kind, class Token and TokenStream.
 *
 *	Token "kinds", streams and symbol table.
 *
 *	Created by Randy Merkel on 6/7/2013.
 *  Copyright (c) 2016 Randy Merkel. All rights reserved.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <cstdint>
#include <map>
#include <sstream>

#include "pl0.h"

/************************************************************************************************
 *	Token "kinds" (types)																		*
 ************************************************************************************************/

/// Token kinds are representated by the integer value of its character
enum class Kind : char {
	none,								///< Placeholder for a 'real' token
	ident,								///< An identifier
	constant,							///< A constant identifier
	undefined,							///< An undefined identifier
	number,								///< floating-point literal
	equal,								///< Is equal
	nequal,								///< Not equal
	lte,								///< Less than or equal
	gte,								///< Greater then or equal
	ConstDecl,							///< "const"
	VarDecl,							///< "var"
	ProcDecl,							///< "procedure"
	Call,								///< "call"
	Begin,								///< "begin"
	End,								///< "end"
	If,									///< "if"
	Then,								///< "then"
	Else,								///< "else"
	While,								///< "while"
	Do,									///< "do"
	Odd,								///< "odd"

	eos,								///< End of stream

	// End of non-printing character codes for ASCII and UNICODE

	Not		= '!',						///< Logical not

	mod 	= '%',						///< Modulus (remainder)

	lp		= '(',						///< Opening parentheses
	rp		= ')',						///< Closing parentheses
	mul		= '*',						///< Multiplication
	plus	= '+',						///< Addition
	comma	= ',',						///< Decl separator
	minus	= '-',						///< Subtraction
	period	= '.',						///< Period
	div 	= '/',						///< Division

	scolon	= ';',						///< Statement separator
	lthan	= '<',						///< Less than 
	assign	= '=',						///< Assignment
	gthan	= '>',						///< Greater then

	expo	= '^',						///< Exponentiation
};

std::string String(Kind k);

/************************************************************************************************
 *	Symbol table
 ************************************************************************************************/

/// A Symbol table entry
struct SymValue {
	Kind		kind;							///< ident, constant, Proc or undefined
	unsigned	level;							///< If kind == Proc
	pl0::Word	value;							///< value (ident, constant) or address (Proc)

	/// Default construction
	SymValue() : kind {Kind::undefined}, level {0}, value{0}			{}

	/// Constructor; create a SymValue from it's components
	SymValue(Kind k, unsigned l = 0, pl0::Word v = 0) : kind{k}, level{l}, value{v}
		{}
};

/// A SymbolTable
typedef std::map<std::string, SymValue> SymbolTable;

/************************************************************************************************
 *	Token Stream																				*
 ************************************************************************************************/

/// A token kind/value pair
struct Token {
	Kind		kind;					///< Token type
	std::string	string_value;			///< kind == ident
	pl0::Word	number_value;			///< Kind == number

	/// Construct a token of type k, stirng value "", number value 0.
    Token(Kind k) : kind{k}, number_value{0} {}
};

/** A restartable stream of tokens
 *
 *	Maintains the current Token read from the input stream.
 *
 *	Token streams may span multiple inputs; when the end of one input is seen, the current
 *	Token is equal to end of stream (Kind::end), a new input source maybe set via
 *	set_input(); get() will return the first Token of the new input.
 */
class TokenStream {
public:
	SymbolTable		table;				///< The symbol table
	size_t			lineNum;			///< Line number of the current stream

	/// Initialize with an input stream which this does not own
	TokenStream(std::istream& s) : lineNum{1}, ip{&s}, owns{false}	{}

	/// Initialize with an input stream which this does own
	TokenStream(std::istream* s) : lineNum{1}, ip{s}, owns{true}	{}

	~TokenStream()						{	close();	}

	Token get();

	/// The current token
	Token& current() 					{	return ct;	}

	void set_input(std::istream& s) {	///< Set the input stream to s
		close();
		ip = &s;
		owns = false;
		lineNum = 1;
	}

	void set_input(std::istream* p) {	///< Set the input stream to s
		close();
		ip = p;
		owns = true;
		lineNum = 1;
	}

private:
	std::istream*	ip;					///< Poiner to an input stream
	bool			owns;				///< Does the TokenStream own this istream?
	Token 			ct { Kind::none };	///< Current token

	/// If this owns ip, delete it.
	void close()						{ if (owns) delete ip;	}
};

#endif
