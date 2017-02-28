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

#include "pl0c.h"

/// A token kind/value pair
struct Token {
	/** Token kinds
	 *  
	 *  Token kinds are divided up into keywords, operators, identifiers and
	 *  numbers. Single character tokens are represented by the integer value of
	 *  its character
	 */
	enum Kind : char {
		unknown,						///< Unknown token kind; (number_value)
		badComment,						///> Unterminated comment, started at line # (number_value)
			
		identifier,	  		  			///< An identifier (string_value)
		number,							///< literal number (number_value)
											
		equ,							///< Is equal? (==)
		neq,							///< Not equal? (!=)
		lte,							///< Less than or equal? (<=)
		gte,							///< Greater then or equal? (>=)
		lor,							///< Or? (||)
		land,							///< And? (&&)
		
		constDecl,						///< "const"
		varDecl,						///< "var"
		procDecl,						///< "procedure"
		funcDecl,						///< "function"
		call,							///< "call"
		begin,							///< "begin"
		end,							///< "end"
		If,								///< "if"
		then,							///< "then"
		Else,							///< "else"
		While,							///< "while"
		Do,								///< "do"
		repeat,							///< "repeat"
		until,							///< "until"
		odd,							///< "odd"
										
		eof,							///< End of stream

		// End of non-printing character codes for ASCII and UNICODE (ordinal value 32)

		Not			= '!',				///< Logical not

		mod			= '%',				///< Modulus (remainder)

		lparen		= '(',				///< Opening parentheses
		rparen		= ')',				///< Closing parentheses
		mul			= '*',				///< Multiplication
		add			= '+',				///< Addition
		comma		= ',',				///< Decl separator
		sub			= '-',				///< Subtraction
		period		= '.',				///< Period
		div 		= '/',				///< Division

		semicolon	= ';',				///< Statement separator
		lt			= '<',				///< Less than
		assign		= '=',				///< Assignment
		gt			= '>',				///< Greater then

		bxor		= '^',				///< Bit XOR

		bor			= '|',				///< Bit or
		band		= '&'				///< Bit and

	};

	static std::string toString(Kind k); ///< Return k's name

	Kind		kind;					///< Token type
	std::string	string_value;			///< kind == ident
	pl0c::Word	number_value;			///< Kind == number

	/// Construct a token of type k, stirng value "", number value 0.
    Token(Kind k) : kind{k}, number_value{0} {}
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
										/// A map of keywords to their 'kind'
	typedef	std::map<std::string, Token::Kind> KeywordTable;
	static	KeywordTable	keywords;	///< The keyword table

	std::istream*	ip;					///< Pointer to an input stream
	bool			owns;				///< Does *this* own ip?
	size_t 			col;				///< Index into line for next character
	std::string 	line;				///< last line read from the stream 

	/// The current token
	Token 			ct { Token::Kind::eof };

	/// If *this* owns ip, delete it.
	void close()							{ if (owns) delete ip;	}
};

#endif
