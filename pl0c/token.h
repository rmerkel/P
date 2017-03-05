/**	@file	token.h
 * 
 *  The PL/0C scanner.
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

#include <cstdint>
#include <map>
#include <sstream>

#include "pl0c.h"

namespace pl0c {
	/// A token "kind"/value pair
	struct Token {
		/** Token kinds
		 *  
		 *  Token kinds are divided up into keywords, operators, identifiers and
		 *  numbers. Single character tokens are represented by the integer value of
		 *  its character
		 */
		enum Kind : char {
			unknown,						///< Unknown token kind; (number_value)
			badComment,						///< Unterminated comment, started at line # (number_value)

			eof,							///< End of stream

			identifier,	  		  			///< An identifier (string_value)
			number,							///< literal number (number_value)									

			equ,							///< Is equal? (==)
			neq,							///< Not equal? (!=)
			lte,							///< Less than or equal? (<=)
			gte,							///< Greater then or equal? (>=)
			lor,							///< Or? (||)
			land,							///< And? (&&)

			lshift,							///< Left shift "<<"
			rshift,							///< Right shift ">>"

			constDecl,						///< "const" constant declaration
			varDecl,						///< "var" variable (mutable) declaration
			procDecl,						///< "procedure" declaraction
			funcDecl,						///< "function" declaration
			begin,							///< "begin" ... "end"
			end,							///< "end"
			If,								///< "if" condition "then" ...
			then,							///< "then"
			Else,							///< "else"
			While,							///< "while" ... "do"
			Do,								///< "do"
			repeat,							///< "repeat" ... "until"
			until,							///< "until"

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
			band		= '&',				///< Bit and

			comp		= '~'				///< 1's complament
		};

		static std::string toString(Kind k); ///< Return k's name

		Kind			kind;				///< Token type
		std::string		string_value;		///< kind == ident
		pl0c::Integer	number_value;		///< Kind == number

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
		Token 			ct { Token::Kind::eof };

		/// If *this* owns ip, delete it.
		void close()							{ if (owns) delete ip;	}
	};
}

#endif
