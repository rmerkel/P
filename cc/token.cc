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
 *	Kind
 ************************************************************************************************/

/// Return a string with k's name
string String(Kind k) {
	switch(k) {
	case Kind::none:		return "none";		break;
	case Kind::ident:		return "ident";		break;
	case Kind::constant:	return "constant";	break;
	case Kind::undefined:	return "undefined";	break;
	case Kind::number:		return "number";	break;
	case Kind::equal:		return "==";		break;
	case Kind::nequal:		return "!=";		break;
	case Kind::lte:			return "<=";		break;
	case Kind::gte:			return ">=";		break;
	case Kind::ConstDecl:	return "const";		break;
	case Kind::VarDecl:		return "var";		break;
	case Kind::ProcDecl:	return "procedure";	break;
	case Kind::Call:		return "call";		break;
	case Kind::Begin:		return "begin";		break;
	case Kind::End:			return "end";		break;
	case Kind::If:			return "if";		break;
	case Kind::Then:		return "then";		break;
	case Kind::Else:		return "else";		break;
	case Kind::While:		return "while";		break;
	case Kind::Do:			return "do";		break;
	case Kind::Odd:			return "odd";		break;
	case Kind::eos:			return "eos";		break;
	case Kind::mod:			return "%";			break;
	case Kind::lp:			return "(";			break;
	case Kind::rp:			return ")";			break;
	case Kind::mul:			return "*";			break;
	case Kind::plus:		return "+";			break;
	case Kind::comma:		return ",";			break;
	case Kind::minus:		return "-";			break;
	case Kind::period:		return ".";			break;
	case Kind::div:			return "/";			break;
	case Kind::scolon:		return ";";			break;
	case Kind::lthan:		return "<";			break;
	case Kind::assign:		return "=";			break;
	case Kind::gthan:		return ">";			break;
	case Kind::expo:		return "^";			break;
	default: {
			ostringstream oss;
			oss << "Unknown Kind: " << static_cast<unsigned>(k) <<  "!" << ends;
			return oss.str();
		}
	}
}


/************************************************************************************************
 *	Token Stream																				*
 ************************************************************************************************/

// private:

/// Read and return the next token
Token TokenStream::get() {
	char ch = 0;

	do {								// skip whitespace... 
		if (!ip->get(ch))
			return ct = { Kind::eos };

		if (ch == '\n') ++lineNum;		// Count lines...

	} while (isspace(ch));

	switch (ch) {
	case '=':							// = or ==?
		if (!ip->get(ch))	ct.kind = Kind::assign;
		else if ('=' == ch) ct.kind = Kind::equal;
		else {
			ip->putback(ch);
			ct.kind = Kind::assign;
		}
		return ct;
		break;

	case '!':							// ! or !=?
		if (!ip->get(ch))	ct.kind = Kind::Not;
		else if ('=' == ch) ct.kind = Kind::nequal;
		else {
			ip->putback(ch);
			ct.kind = Kind::Not;
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
		case '<':
		case '>':
		case '^':
			return ct = { static_cast<Kind>(ch) };

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			ip->putback(ch);
			*ip >> ct.number_value;
			ct.kind = Kind::number;
			return ct;

		default:							// ident, ident = or error
			if (isalpha(ch)) {
				ct.string_value = ch;
				while (ip->get(ch) && isalnum(ch))
					ct.string_value += ch;

				ip->putback(ch);
				SymValue& v = table[ct.string_value];
				if (v.kind == Kind::undefined || v.kind == Kind::constant)
					ct.kind = Kind::ident;	// possible undefined or constant identifier
				else
					ct.kind = v.kind;
				return ct;
			}

			cerr << "bad token: \'" << ch << "\'\n";	// TBD: Kind::other or bad??
			return ct = { Kind::eos };
	}
}
