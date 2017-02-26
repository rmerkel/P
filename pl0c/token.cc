/**	@file	token.cc
 *
 *	@brief	TokenStream implementation.
 *
 *	Created by Randy Merkel on 6/7/2013.
 *  Copyright (c) 2016 Randy Merkel. All rights reserved.
 */

#include "token.h"

#include <iostream>
#include <sstream>

using namespace std;

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

	do {								// skip whitespace... 
		if (!getch(ch))
			return ct = { Token::eof };

		if ('\n' == ch) ++lineNum;		// Count lines

	} while (isspace(ch));

	switch (ch) {
#if	0	// bit And or logial And... TBD
	case '&':							// & or &&
		if (!getch(ch))	ct.kind = Token::bitAnd;
		else if ('&' == ch)	ct.kind = Token::And;
		else {
			unget();
			ct.kind = Token::bitAnd;
		}
		break;
#endif

	case '=':							// = or ==?
		if (!getch(ch))	ct.kind = Token::assign;
		else if ('=' == ch) ct.kind = Token::equ;
		else {
			unget();
			ct.kind = Token::assign;
		}
		return ct;
		break;

	case '!':							// ! or !=?
		if (!getch(ch))	ct.kind = Token::Not;
		else if ('=' == ch) ct.kind = Token::neq;
		else {
			unget();
			ct.kind = Token::Not;
		}
		return ct;
		break;

	case '>':							// > or >=?
		if (!getch(ch))	ct.kind = Token::gt;
		else if ('=' == ch)	ct.kind = Token::gte;
		else {
			unget();
			ct.kind = Token::gt;
		}
		return ct;
		break;

	case '<':							// < or <=?
		if (!getch(ch))	ct.kind = Token::lt;
		else if ('=' == ch)	ct.kind = Token::lte;
		else {
			unget();
			ct.kind = Token::lt;
		}
		return ct;
		break;

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
		}

		cerr << "bad token: \'" << ch << "\'\n";
		return ct = { Token::eof };
	}
}

// public static

/// Return a string with k's name
string Token::toString(Token::Kind k) {
	switch(k) {
	case Kind::identifier:	return "identifier";	break;
	case Kind::number:		return "number";		break;
	case Kind::equ:			return "==";			break;
	case Kind::neq:			return "!=";			break;
	case Kind::lte:			return "<=";			break;
	case Kind::gte:			return ">=";			break;
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
	case Kind::odd:			return "odd";			break;
	case Kind::Not:			return "not";			break;
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
	case Kind::expo:		return "^";				break;
	case Kind::eof:			return "eof";			break;
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
	{	"odd",			Token::odd			},
	{	"mod",			Token::mod			}
};
