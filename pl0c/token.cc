/**	@file	token.cc
 *
 *  TokenStream implementation. Started life as the Token and TokenStream classes from The C++
 *  Programming Language, 4th Edition, by Stroustrup, and then modified to work on the integer
 *  based PL/0C machine.
 *
 *	Created by Randy Merkel on 6/7/2013.
 *  Copyright (c) 2016 Randy Merkel. All rights reserved.
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
				return ct = { Token::eof };

			if ('\n' == ch) ++lineNum;				// Count lines

		} while (isspace(ch));

		switch (ch) {
		case '=':									// = or ==?
			if (!getch(ch))		ct.kind = Token::assign;
			else if ('=' == ch) ct.kind = Token::equ;
			else {	unget();	ct.kind = Token::assign;	}
			return ct;

		case '!':									// ! or !=?
			if (!getch(ch))		ct.kind = Token::Not;
			else if ('=' == ch) ct.kind = Token::neq;
			else {	unget();	ct.kind = Token::Not;	}
			return ct;

		case '>':									// >, >> or >=?
			if (!getch(ch))		ct.kind = Token::gt;
			else if ('>' == ch)	ct.kind = Token::rshift;
			else if ('=' == ch)	ct.kind = Token::gte;
			else {	unget();	ct.kind = Token::gt;	}
			return ct;

		case '<':									// <, << or <=?
			if (!getch(ch))		ct.kind = Token::lt;
			else if ('<' == ch)	ct.kind = Token::lshift;
			else if ('=' == ch)	ct.kind = Token::lte;
			else {	unget();	ct.kind = Token::lt;	}
			return ct;

		case '|':							// | or ||?
			if (!getch(ch)) 	ct.kind = Token::bor;
			else if ('|' == ch) ct.kind = Token::lor;
			else {	unget();	ct.kind = Token::gt;	}
			return ct;

		case '&':							// & or &&?
			if (!getch(ch)) 	ct.kind = Token::band;
			else if ('&' == ch) ct.kind = Token::land;
			else {	unget();	ct.kind = Token::band;  }
			return ct;

		case '{':									// comment; { ... }
			ct.number_value = lineNum;				// remember where the comment stated...
			do {									// eat everthhing up to the closing '}' 
				if (!getch(ch)) {
					ct.kind = Token::badComment;
					return ct;
				}

				if ('\n' == ch) 
					++lineNum;						// keep counting lines...

			} while ('}' != ch);
			return get();							// restart the scan..

		case '%':
		case '(':
		case ')':
		case '*':
		case '+':
		case ',':
		case '-':
		case '.':
		case '/':
		case ';':
		case '^':
			return ct = { static_cast<Token::Kind>(ch) };

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			ct.string_value = ch;
			while (getch(ch) && isdigit(ch))
				ct.string_value += ch;
			unget();
			std::istringstream iss (ct.string_value);
			iss >> ct.number_value;
			ct.kind = Token::number;
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
					ct.kind = Token::identifier;
				return ct;

			} else {

				ct.string_value = ch;
				ct.number_value = ch;
				ct.kind = Token::unknown;
				return ct;
			}
		}
	}

	// public static

	/// Return a string with k's name
	string Token::toString(Token::Kind k) {
		switch(k) {
		case Kind::unknown:		return "unknown";		break;
		case Kind::badComment:	return "bad comment";	break;
		case Kind::identifier:	return "identifier";	break;
		case Kind::number:		return "number";		break;
		case Kind::equ:			return "==";			break;
		case Kind::neq:			return "!=";			break;
		case Kind::lte:			return "<=";			break;
		case Kind::gte:			return ">=";			break;
		case Kind::lor:			return "||";			break;
		case Kind::land:		return "&&";			break;
		case Kind::constDecl:	return "const";			break;
		case Kind::varDecl:		return "var";			break;
		case Kind::procDecl:	return "procedure";		break;
		case Kind::funcDecl:	return "function";		break;
		case Kind::begin:		return "begin";			break;
		case Kind::end:			return "end";			break;
		case Kind::If:			return "if";			break;
		case Kind::then:		return "then";			break;
		case Kind::Else:		return "else";			break;
		case Kind::While:		return "while";			break;
		case Kind::Do:			return "do";			break;
		case Kind::repeat:		return "repeat";		break;
		case Kind::until:		return "until";			break;
		case Kind::eof:			return "eof";			break;

		case Kind::Not:			return "!";				break;
		case Kind::mod:			return "%";				break;
		case Kind::lparen:		return "(";				break;
		case Kind::rparen:		return ")";				break;
		case Kind::mul:			return "*";				break;
		case Kind::add:			return "+";				break;
		case Kind::comma:		return ",";				break;
		case Kind::sub:			return "-";				break;
		case Kind::period:		return ".";				break;
		case Kind::div:			return "/";				break;
		case Kind::semicolon:	return ";";				break;
		case Kind::lt:			return "<";				break;
		case Kind::assign:		return "=";				break;
		case Kind::gt:			return ">";				break;
		case Kind::bxor:		return "^";				break;
		case Kind::bor:			return "|";				break;
		case Kind::band:		return "&";				break;

		default: {
				ostringstream oss;
				oss << "Unknown Kind: " << static_cast<unsigned>(k) <<  "!" << ends;
				return oss.str();
			}
		}
	}

	// privite static

	TokenStream::KeywordTable	TokenStream::keywords = {
		{   "const",		Token::constDecl	},
		{	"var",			Token::varDecl		},
		{	"procedure",	Token::procDecl		},
		{	"function",		Token::funcDecl		},
		{	"call",			Token::call			},
		{	"begin",		Token::begin		},
		{	"end",			Token::end			},
		{	"if",			Token::If			},
		{	"then",			Token::then			},
		{	"else",			Token::Else			},
		{	"while",		Token::While		},
		{	"do",			Token::Do			},
		{	"repeat",		Token::repeat		},
		{	"until",		Token::until		},
		{	"mod",			Token::mod			}
	};
}
