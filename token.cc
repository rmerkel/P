/**	@file	token.cc
 *
 * TokenStream implementation. Started life as the Token and TokenStream classes from The C++
 * Programming Language, 4th Edition, by Stroustrup, and then modified to work on the integer based
 * PL/0C machine.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "token.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace pl0c {
	/************************************************************************************************
	 *	Token 																						*
	 ************************************************************************************************/

	// private:

	std::istream& TokenStream::getch(char& c) {
		if (col == line.size()) {
			col = 0;
			line.clear();
			if (getline(*ip, line))
				line.push_back('\n');
		}

		if (col < line.size())
			c = line[col++];

		return *ip;
	}

	std::istream& TokenStream::unget() {
		if (col > 0) --col;
		return *ip;
	}

	/// Read and return the next token
	Token TokenStream::get() {
		char ch = 0;

		do {										// skip whitespace...
			if (!getch(ch))
				return ct = { Token::EOS };

			if ('\n' == ch) ++lineNum;				// Count lines

		} while (isspace(ch));

		switch (ch) {
		case '=':									// = or ==?
			if (!getch(ch))		ct.kind = Token::Assign;
			else if ('=' == ch) ct.kind = Token::EQU;
			else {	unget();	ct.kind = Token::Assign;	}
			return ct;

		case '!':									// ! or !=?
			if (!getch(ch))		ct.kind = Token::NOT;
			else if ('=' == ch) ct.kind = Token::NEQU;
			else {	unget();	ct.kind = Token::NOT;	}
			return ct;

		case '>':									// >, >> or >=?
			if (!getch(ch))		ct.kind = Token::GT;
			else if ('>' == ch)	ct.kind = Token::ShiftR;
			else if ('=' == ch)	ct.kind = Token::GTE;
			else {	unget();	ct.kind = Token::GT;	}
			return ct;

		case '<':									// <, << or <=?
			if (!getch(ch))		ct.kind = Token::LT;
			else if ('<' == ch)	ct.kind = Token::ShiftL;
			else if ('=' == ch)	ct.kind = Token::LTE;
			else {	unget();	ct.kind = Token::LT;	}
			return ct;

		case '|':							// | or ||?
			if (!getch(ch)) 	ct.kind = Token::BitOR;
			else if ('|' == ch) ct.kind = Token::OR;
			else {	unget();	ct.kind = Token::GT;	}
			return ct;

		case '&':							// & or &&?
			if (!getch(ch)) 	ct.kind = Token::BitAND;
			else if ('&' == ch) ct.kind = Token::AND;
			else {	unget();	ct.kind = Token::BitAND;  }
			return ct;

		case '{':									// comment; { ... }
			ct.integer_value = lineNum;				// remember where the comment stated...
			do {									// eat everthhing up to the closing '}'
				if (!getch(ch)) {
					ct.kind = Token::BadComment;
					return ct;
				}

				if ('\n' == ch)
					++lineNum;						// keep counting lines...

			} while ('}' != ch);
			return get();							// restart the scan..

		case '%': case '(': case ')': case '*':
		case '+': case ',': case '-': case '/':
		case ':': case ';': case '^':
			return ct = { static_cast<Token::Kind>(ch) };

		case '.': 									// real number, or just a '.'
			ct.string_value = ch;
			if (!getch(ch) || !isdigit(ch))
				ct.kind = Token::Period;

			else {									// Real...
				ct.string_value = ch;
				ct.kind = Token::RealNum;
				while (getch(ch) && (isdigit(ch) || 'e' == ch || 'E' == ch))
					ct.string_value += ch;
				unget();

				std::istringstream iss (ct.string_value);
				iss >> ct.real_value;
				return ct;
			}
			return ct;
													// integer or real number
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			ct.kind = Token::IntegerNum;			// Assume integer value...
			ct.string_value = ch;
			while (getch(ch)) {
				if ('.' == ch || 'e' == ch || 'E' == ch)
					ct.kind = Token::RealNum;

				else if (!isdigit(ch))
					break;

				ct.string_value += ch;
			}
			unget();

			std::istringstream iss (ct.string_value);
			if (Token::RealNum == ct.kind)
				iss >> ct.real_value;

			else
				iss >> ct.integer_value;
			return ct;
		}

		default:							// ident, ident = or error
			if (isalpha(ch)) {
				ct.string_value = ch;
				while (getch(ch) && isalnum(ch))
					ct.string_value += ch;

				unget();
				auto it = keywords.find(ct.string_value);
				if (keywords.end() != it)
					ct.kind = it->second;
				else
					ct.kind = Token::Identifier;
				return ct;

			} else {
				ct.string_value = ch;
				ct.integer_value = ch;
				ct.kind = Token::Unknown;
				return ct;
			}
		}
	}

	// public static

	/// Return a string with k's name
	string Token::toString(Token::Kind k) {
		switch(k) {
		case Kind::Unknown:		return "unknown";		break;
		case Kind::BadComment:	return "bad comment";	break;

		case Kind::Identifier:	return "identifier";	break;
		case Kind::IntegerNum:	return "IntegerNum";	break;
		case Kind::RealNum:		return "RealNum";		break;

		case Kind::ConsDecl:	return "const";			break;
		case Kind::VarDecl:		return "var";			break;
		case Kind::ProcDecl:	return "procedure";		break;
		case Kind::FuncDecl:	return "function";		break;
		case Kind::Begin:		return "begin";			break;
		case Kind::End:			return "end";			break;
		case Kind::If:			return "if";			break;
		case Kind::Then:		return "then";			break;
		case Kind::Else:		return "else";			break;
		case Kind::While:		return "while";			break;
		case Kind::Do:			return "do";			break;
		case Kind::Repeat:		return "repeat";		break;
		case Kind::Until:		return "until";			break;

		case Kind::Integer:		return "integer";		break;
		case Kind::Real:		return "real";			break;

		case Kind::EOS:			return "EOS";			break;

		case Kind::EQU:			return "==";			break;
		case Kind::LTE:			return "<=";			break;
		case Kind::GTE:			return ">=";			break;
		case Kind::OR:			return "||";			break;
		case Kind::AND:			return "&&";			break;
		case Kind::NEQU:		return "!=";			break;

		case Kind::NOT:			return "!";				break;
		case Kind::LT:			return "<";				break;
		case Kind::GT:			return ">";				break;

		case Kind::BitXOR:		return "^";				break;
		case Kind::BitOR:		return "|";				break;
		case Kind::BitAND:		return "&";				break;

		case Kind::Add:			return "+";				break;
		case Kind::Subtract:	return "-";				break;
		case Kind::Multiply:	return "*";				break;
		case Kind::Divide:		return "/";				break;
		case Kind::Mod:			return "%";				break;

		case Kind::OpenParen:	return "(";				break;
		case Kind::CloseParen:	return ")";				break;
		case Kind::Comma:		return ",";				break;
		case Kind::Period:		return ".";				break;
		case Kind::Colon:		return ":";				break;
		case Kind::SemiColon:	return ";";				break;
		case Kind::Assign:		return "=";				break;

		default: {
				ostringstream oss;
				oss << "Unknown Kind: " << static_cast<unsigned>(k) <<  "!" << ends;
				return oss.str();
			}
		}
	}

	/************************************************************************************************
	 *	TokenStream
	 ************************************************************************************************/

	// public

	/**
	 * Set the input stream to a reference to s.
	 * @param	s	The new input stream
	 */
	void TokenStream::set_input(std::istream& s) {
		close();
		ip = &s;
		owns = false;
		lineNum = 1;
	}

	/**
	 * Set th input stream to p, taking ownership of that stream.
	 * @param	p	The new input stream
	 */
	void TokenStream::set_input(std::istream* p) {
		close();
		ip = p;
		owns = true;
		lineNum = 1;
	}

	// privite static

	TokenStream::KeywordTable	TokenStream::keywords = {
		{   "const",		Token::ConsDecl	},
		{	"var",			Token::VarDecl		},
		{	"procedure",	Token::ProcDecl		},
		{	"function",		Token::FuncDecl		},
		{	"begin",		Token::Begin		},
		{	"end",			Token::End			},
		{	"if",			Token::If			},
		{	"then",			Token::Then			},
		{	"else",			Token::Else			},
		{	"while",		Token::While		},
		{	"do",			Token::Do			},
		{	"repeat",		Token::Repeat		},
		{	"until",		Token::Until		},
		{	"mod",			Token::Mod			},
		{	"integer",		Token::Integer		},
		{	"real",			Token::Real			}
	};
}
