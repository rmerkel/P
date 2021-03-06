/********************************************************************************************//**
 *	@file	token.h
 *
 * The P Language scanner.
 *
 * The scanner started life as the Token and TokenStream classes for the calulator example from
 * The C++ Programming Language, 4th Edition, by Stroustrup, modified in an initial port of
 * Wirth's PL/0 compilier, and then transformed to use more C/C++ operands yielding the Pascal 
 * scanner and the the P scanner.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef TOKEN_H
#define TOKEN_H

#include "datum.h"

#include <iostream>
#include <map>
#include <sstream>
#include <set>

/********************************************************************************************//**
 * A token "kind"/value pair
 ************************************************************************************************/
struct Token {
	/** Token kinds
	 *
	 * Token kinds are divided up into keywords, operators, identifiers and numbers. Built-in
	 * types, such as "integer" or "real" are installed directly into the symbol table, and thus 
	 * don't have a Token Kind.
	 */
	enum Kind {
		Unknown,						///< Unknown token kind; (integer_value)
		BadComment,						///< Unterminated comment, started at line # (integer_value)

		Identifier,	  		  			///< An identifier (string_value)
		Character,						///< A single character, e.g., string_value[0] = 'c'
		String,							///< A string (string_value), "string"
		IntegerNum,						///< Integer literal number (integer_value)
		RealNum,						///< Real literal number (real_value)

		ConsDecl,						///< "const" constant declaration
		FuncDecl,						///< "function" declaration
		ProcDecl,						///< "procedure" declaraction
		ProgDecl,						///< "program" declaraction
		TypeDecl,						///< "type" declaraction
		VarDecl,						///< "var" variable (mutable) declaration

		Begin,							///< "begin" ... "end"
		End,							///< "end"
		Endfunc,						///< "endfunc"
		Endif,							///< "endif"
		Endloop,						///< "endloop"
		Endproc,						///< "endproc"
		Endprog,						///< "endprog"
		If,								///< "if" condition "then" ...
		In,								///< "for" ident "in" ...
		Then,							///< "then"
		Elif,							///< "elif"
		Else,							///< "else"
		Loop,							///< "loop"
		While,							///< "while" ... "do"
		Repeat,							///< "repeat" ... "until"
		Return,							///< "return" expr
		Reverse,						///< "for" ident "in" "reverse" ...
		Until,							///< "until"
		For,							///< "for"
		Is,								///< "is"

		Ellipsis,						///< ".."
		Caret,							///< "^"

		Array,							///< "array"
		Of,								///< "of"
		Record,							///< "record ... end"

		LT,								///< Less than
		LTE,							///< Less than or equal? (<=)
		EQU,							///< Is equal? (=)
		GTE,							///< Greater then or equal? (>=)
		GT,								///< Greater then
		NEQ,                           	///< Not equal? (<>)

		And,							///< Logical And
		Or,								///< Logical Or
		Not,							///< Logical Not

		BitNot,							///< Bitwise Not
		BitAnd,							///< Bitwise And
		BitOr,							///< Bitwise Or
		BitXor,							///< Bitwise Xor

		ShiftLeft,						///< Bitwise shift left
		ShiftRight,						///< Bitwise shift right

		Add,							///< Addition
		Subtract,						///< Subtraction
		Multiply,						///< Multiplication
		Divide,							///< Division

		OpenParen,						///< Opening parentheses
		CloseParen,						///< Closing parentheses
		OpenBrkt,						///< Opening bracket
		CloseBrkt,						///< Closing bracket

		Comma,							///< ',' - Decl separator
		Period,							///< '.'
		Colon,							///< Identifier ':' type
		SemiColon,						///< ';' - Statement separator
		Tick,							///< '`' - Attribute selector

		// built-in functions...

		Round,							///< round real to integer
		Trunc,							///< truncate real to integer
		Abs,							///< Absolute value
		Atan,							///< Arc tangent
		Exp,							///< e to the given power 
		Log,							///< natural log
		Odd,							///< Evenly divisable by 2?
		Pred,							///< Previous ordinal of value

		Sin,							///< Sine of value
		Sqr,							///< Square of value
		Sqrt,							///< Square root of value
		Succ,							///< Next ordinal of value

		Get,							///< Read a value from standard input
		Put,							///< Write on standard output
		Putln,							///< Write on standard output, plus newline
		New,							///< Allocate dynamic store
		Dispose,						///< Free allocated dynamic store

		Assign,							///< Assignment (:=)
		Mod,							///< Modulus (remainder)
		Ord,							///< Convert ordinal value to ordinal

		EOS			= 127				///< End of stream

	};

	typedef std::set<Kind>	KindSet;	///< Set of Token kinds...

	Kind			kind;				///< Token type
	std::string		string_value;		///< kind == Identifier, or String or Character
	int				integer_value;      ///< Kind == IntegerNum
	double			real_value;			///< Kind == RealNum

	/// Construct a token of type k, stirng value "", number value 0.
	Token(Kind k) : kind{k}, integer_value{0}, real_value{0.0} {}
	virtual ~Token() {}					///< Destructor
};

/********************************************************************************************//**
 * A restartiable stream of tokens
 *
 * Maintains the last Token read from the input stream.
 *
 * Token streams may span multiple inputs; when the end of one input is seen, the current Token
 * is equal to end of stream (Kind::end), a new input source maybe set via set_input(); get()
 * will return the first Token of the new input.
 ************************************************************************************************/
class TokenStream {
public:
	size_t			lineNum;			///< Line # of the current stream

	/// Initialize with an input stream which this does not own
	TokenStream(std::istream& s) : lineNum{1}, ip{&s}, owns{false}, col{0}	{}

	/// Initialize with an input stream which this does own
	TokenStream(std::istream* s) : lineNum{1}, ip{s}, owns{true}, col{0}	{}

	/// Destructor
	virtual ~TokenStream()				{	close();	}

	/// Return the next character from the stream.
	std::istream& getch(char& c);
	std::istream& unget();				///< Return last character to the stream

	Token get();

	/// The current token
	Token& current() 					{	return ct;	}

	void set_input(std::istream& s);	///< Set the input stream to s
	void set_input(std::istream* p);	///< Set the input stream to p

private:								/// A map of keywords to their 'kind'
	typedef	std::map<std::string, Token::Kind> KeywordTable;

	static	KeywordTable	keywords;	///< The keyword table

	std::istream*	ip;					///< Pointer to an input stream
	bool			owns;				///< Does *this* own ip?
	size_t 			col;				///< Index into line for next character
	std::string 	line;				///< last line read from the stream

	/// The current token
	Token 			ct { Token::EOS };

	/// If *this* owns ip, delete it.
	void close()						{	if (owns) delete ip;	}
};

/********************************************************************************************//**
 * Kind put operator
 ************************************************************************************************/
std::ostream& operator<<(std::ostream& os, const Token::Kind& kind);

#endif
