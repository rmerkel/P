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
			Unknown,						///< Unknown token kind; (number_value)
			BadComment,						///< Unterminated comment, started at line # (number_value)

			Identifier,	  		  			///< An identifier (string_value)
			Number,							///< literal number (number_value)
			Constant,						///< "const" constant declaration
			Variable,						///< "var" variable (mutable) declaration
			Procedure,						///< "procedure" declaraction
			Function,						///< "function" declaration
			Begin,							///< "begin" ... "end"
			End,							///< "end"
			If,								///< "if" condition "then" ...
			Then,							///< "then"
			Else,							///< "else"
			While,							///< "while" ... "do"
			Do,								///< "do"
			Repeat,							///< "repeat" ... "until"
			Until,							///< "until"

			EQU,							///< Is equal? (==)
			LTE,							///< Less than or equal? (<=)
			GTE,							///< Greater then or equal? (>=)
			OR,								///< Or? (||)
			AND,							///< And? (&&)
			NEQU,							///< Not equal? (!=)

			ShiftL,							///< Left shift "<<"
			ShiftR,							///< Right shift ">>"

			EOS,							///< End of stream

			// End of non-printing character codes for ASCII and UNICODE (ordinal value 32)

			Not			= '!',				///< Logical not
			LessThan	= '<',				///< Less than
			GreaterThan	= '>',				///< Greater then

			BitXOR		= '^',				///< Bit XOR
			BitOR		= '|',				///< Bit or
			BitAND		= '&',				///< Bit and
			Complament	= '~',				///< 1's complament

			Add			= '+',				///< Addition
			Subtract	= '-',				///< Subtraction
			Multiply	= '*',				///< Multiplication
			Divide 		= '/',				///< Division
			Mod			= '%',				///< Modulus (remainder)

			OpenParen	= '(',				///< Opening parentheses
			CloseParen	= ')',				///< Closing parentheses
			Comma		= ',',				///< Decl separator
			Period		= '.',				///< Period
			SemiColon	= ';',				///< Statement separator
			Assign		= '='				///< Assignment

		};

		static std::string toString(Kind k); ///< Return k's name

		Kind			kind;				///< Token type
		std::string		string_value;		///< kind == ident
		int				number_value;		///< Kind == number

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
		Token 			ct { Token::Kind::EOS };

		/// If *this* owns ip, delete it.
		void close()							{ if (owns) delete ip;	}
	};
}

#endif
