/**	@file	token.h
 *
 *  The Pascal-Lite scanner.
 *
 * Started life as the Token and TokenStream classes for the calulator example from The C++
 * Programming Language, 4th Edition, by Stroustrup, modified in an initial port of Wirth's PL/0
 * compilier, and then transformed to use more C/C++ operands yielding the PL/0C scanner.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "datum.h"

#include <map>
#include <sstream>
#include <set>

/// A token "kind"/value pair
struct Token {
	/** Token kinds
	 *
	 *  Token kinds are divided up into keywords, operators, identifiers andNumber
	 *  numbers. Single character tokens are represented by the integer value of
	 *  its character
	 */
	enum Kind : char {
		Unknown,						///< Unknown token kind; (integer_value)
		BadComment,						///< Unterminated comment, started at line # (integer_value)

		Identifier,	  		  			///< An identifier (string_value)
		IntegerNum,						///< Integer literal number (integer_value)
		RealNum,						///< Real literal number (real_value)
		TypeDecl,						///< "type" declaraction
		ConsDecl,						///< "const" constant declaration
		VarDecl,						///< "var" variable (mutable) declaration
		ProgDecl,						///< "program" declaraction
		ProcDecl,						///< "procedure" declaraction
		FuncDecl,						///< "function" declaration
		Begin,							///< "begin" ... "end"
		End,							///< "end"
		If,								///< "if" condition "then" ...
		Then,							///< "then"
		Else,							///< "else"
		While,							///< "while" ... "do"
		Do,								///< "do"
		Repeat,							///< "repeat" ... "until"
		Until,							///< "until"
		Ellipsis,						///< ".."

		IntegerType,					///< "integer"
		RealType,						///< "real"
		Array,							///< "array"
		Of,								///< "of"

		Round,							///< round real to integer

		Assign,							///< Assignment (:=)

		LTE,							///< Less than or equal? (<=)
		GTE,							///< Greater then or equal? (>=)
		Or,								///< Or? 
		And,							///< And?
		NEQ,                           	///< Not equal? (<>)

		ShiftL,							///< Left shift "<<"
		ShiftR,							///< Right shift ">>"Number

		Mod,							///< Modulus (remainder)
		EOS,							///< End of stream

		// End of non-printing character codes for ASCII and UNICODE (ordinal value 32)

		EQU			= '=',				///< Is equal? (=)

		LT			= '<',				///< Less than
		GT			= '>',				///< Greater then

		Add			= '+',				///< Addition
		Subtract	= '-',				///< Subtraction
		Multiply	= '*',				///< Multiplication
		Divide 		= '/',				///< Division

		OpenParen	= '(',				///< Opening parentheses
		CloseParen	= ')',				///< Closing parentheses
		OpenBrkt	= '[',				///< Opening bracket
		CloseBrkt	= ']',				///< Closing bracket
		Comma		= ',',				///< Decl separator
		Period		= '.',				///< Period
		Colon		= ':',				///< Identifier ':' type
		SemiColon	= ';',				///< Statement separator
	};

	/// A set of Token kinds
	typedef std::set<Kind>	KindSet;

	static std::string toString(Kind k); ///< Return k's name

	Kind			kind;				///< Token type
	std::string		string_value;		///< kind == Identifier
	Integer			integer_value;      ///< Kind == IntegerNum
	Real			real_value;			///< Kind == RealNum

	/// Construct a token of type k, stirng value "", number value 0.
	Token(Kind k) : kind{k}, integer_value{0} {}
	virtual ~Token() {}					///< Destructor
};

/** A restartiable stream of tokens
 *
 *	Maintains the last Token read from the input stream.
 *
 *	Token streams may span multiple inputs; when the end of one input is seen,
 *	the current Token is equal to end of stream (Kind::end), a new input source
 *	maybe set via set_input(); get() will return the first Token of the new
 *	input.
 */
class TokenStream {
public:
	size_t			lineNum;			///< Line # of the current stream

	/// Initialize with an input stream which this does not own
	TokenStream(std::istream& s) : lineNum{1}, ip{&s}, owns{false}, col{0}	{}

	/// Initialize with an input stream which this does own
	TokenStream(std::istream* s) : lineNum{1}, ip{s}, owns{true}, col{0}	{}

	/// Destructor
	virtual ~TokenStream()				{	close();	}

	/// Return the next character from the stream. Sets eof in *ip and return *ip
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
	Token 			ct { Token::Kind::EOS };

	/// If *this* owns ip, delete it.
	void close()							{ if (owns) delete ip;	}
};

#endif
