/**	@file	token.cc
 *
 * TokenStream implementation. Started life as the Token and TokenStream classes from The C++
 * Programming Language, 4th Edition, by Stroustrup, and then modified to work on the integer based
 * Pascal-lite machine.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
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

	do {										// skip whitespace...
		if (!getch(ch))
			return ct = { Token::EOS };

		if ('\n' == ch) ++lineNum;				// Count lines

	} while (isspace(ch));

	switch (ch) {
	case '=': return ct.kind = Token::EQU;			break;
	case '+': return ct.kind = Token::Add;			break;
	case '-': return ct.kind = Token::Subtract;		break;
	case '*': return ct.kind = Token::Multiply;		break;
	case '/': return ct.kind = Token::Divide;		break;

	case '(': return ct.kind = Token::OpenParen;	break;
	case ')': return ct.kind = Token::CloseParen;	break;
	case '[': return ct.kind = Token::OpenBrkt;		break;
	case ']': return ct.kind = Token::CloseBrkt;	break;
	case ',': return ct.kind = Token::Comma;		break;
	case ';': return ct.kind = Token::SemiColon;	break;
	case '^': return ct.kind = Token::Caret;		break;

	case '>':									// >, or >=?
		if (!getch(ch))		ct.kind = Token::GT;
		else if ('=' == ch)	ct.kind = Token::GTE;
		else {	unget();	ct.kind = Token::GT;	}
		return ct;

	case '<':									// <, or <=?
		if (!getch(ch))		ct.kind = Token::LT;
		else if ('=' == ch)	ct.kind = Token::LTE;
		else {	unget();	ct.kind = Token::LT;	}
		return ct;

	case ':':									// : or :=?
		if (!getch(ch))		ct.kind = Token::Colon;
		else if ('=' == ch)	ct.kind = Token::Assign;
		else {	unget();	ct.kind = Token::Colon;	}
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

	case '.': 									// '.', or '..'
		if (!getch(ch))		ct.kind = Token::Period;
		else if ('.' == ch)	ct.kind = Token::Ellipsis;
		else {	unget();	ct.kind = Token::Period;	}
		return ct;
												// integer or real number
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9': {
		ct.kind = Token::IntegerNum;			// Assume integer value...
		ct.string_value = ch;
		while (getch(ch)) {
			if ('.' == ch) {
				if (Token::IntegerNum == ct.kind)
					ct.kind = Token::RealNum;

				else {							// Assuming '..' and not 'e.'
					unget();
					ct.string_value.pop_back();
					ct.kind = Token::IntegerNum;
					break;
				}

			} else if ('e' == ch || 'E' == ch)
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
	{	"Abs",			Token::Abs			},
	{	"and",			Token::And			},
	{	"array",		Token::Array		},
	{	"ArcTan",		Token::Atan			},
	{	"begin",		Token::Begin		},
	{   "const",		Token::ConsDecl		},
	{	"Dispose",		Token::Dispose		},
	{	"do",			Token::Do			},
	{	"else",			Token::Else			},
	{	"end",			Token::End			},
	{	"Exp",			Token::Exp			},
	{	"function",		Token::FuncDecl		},
	{	"if",			Token::If			},
	{	"Integer",		Token::IntType		},
	{	"Ln",			Token::Log			},
	{	"mod",			Token::Mod			},
	{	"not",			Token::Not			},
	{	"New",			Token::New			},
	{	"Odd",			Token::Odd			},
	{	"of",			Token::Of			},
	{	"Ord",			Token::Ord			},
	{	"program",		Token::ProgDecl		},
	{	"procedure",	Token::ProcDecl		},
	{	"Pred",			Token::Pred			},
	{	"record",		Token::Record		},
	{	"Real",			Token::RealType		},
	{	"repeat",		Token::Repeat		},
	{	"Round",		Token::Round		},
	{	"Sin",			Token::Sin			},
	{	"Sqr",			Token::Sqr			},
	{	"Sqrt",			Token::Sqrt			},
	{	"Succ",			Token::Succ			},
	{	"then",			Token::Then			},
	{	"Trunc",		Token::Trunc		},
	{	"type",			Token::TypeDecl		},
	{	"until",		Token::Until		},
	{	"for",			Token::For			},
	{	"to",			Token::To			},
	{	"downto",		Token::DownTo,		},
	{	"var",			Token::VarDecl		},
	{	"while",		Token::While		},
	{	"Writeln",		Token::Writeln		}
};

// operators

/********************************************************************************************//**
 * @brief Token::Kind stream put operator
 *
 * @param	os		Stream to write kind to 
 * @param	kind	Token kind to write  or os
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Token::Kind& kind) {
	switch (kind) {
	case Token::Unknown:	os << "unknown";		break;
	case Token::BadComment:	os << "bad comment";	break;

	case Token::Identifier:	os << "identifier";		break;
	case Token::IntegerNum:	os << "IntegerNum";		break;
	case Token::RealNum:	os << "RealNum";		break;

	case Token::ConsDecl:	os << "const";			break;
	case Token::FuncDecl:	os << "function";		break;
	case Token::ProcDecl:	os << "procedure";		break;
	case Token::ProgDecl:	os << "program";		break;
	case Token::TypeDecl:	os << "type";			break;
	case Token::VarDecl:	os << "var";			break;

	case Token::Begin:		os << "begin";			break;
	case Token::End:		os << "end";			break;
	case Token::If:			os << "if";				break;
	case Token::Then:		os << "then";			break;
	case Token::Else:		os << "else";			break;
	case Token::While:		os << "while";			break;
	case Token::Do:			os << "do";				break;
	case Token::Repeat:		os << "repeat";			break;
	case Token::Until:		os << "until";			break;
	case Token::For:		os << "for";			break;
	case Token::To:			os << "to";				break;
	case Token::DownTo:		os << "downto";			break;

	case Token::Ellipsis:	os << "..";				break;
	case Token::Caret:		os << "^";				break;

	case Token::IntType:	os << "Integer";		break;
	case Token::RealType:	os << "Real";			break;

	case Token::Array:		os << "array";			break;
	case Token::Of:			os << "of";				break;
	case Token::Record:		os << "record";			break;

	case Token::LT:			os << "<";				break;
	case Token::LTE:		os << "<=";				break;
	case Token::EQU:		os << "=";				break;
	case Token::GTE:		os << ">=";				break;
	case Token::GT:			os << ">";				break;
	case Token::NEQ:		os << "<>";				break;

	case Token::Or:			os << "or";				break;
	case Token::And:		os << "and";			break;

	case Token::Add:		os << "+";				break;
	case Token::Subtract:	os << "-";				break;
	case Token::Multiply:	os << "*";				break;
	case Token::Divide:		os << "/";				break;

	case Token::OpenParen:	os << "(";				break;
	case Token::CloseParen:	os << ")";				break;
	case Token::OpenBrkt:	os << "[";				break;
	case Token::CloseBrkt:	os << "]";				break;
	case Token::Comma:		os << ",";				break;
	case Token::Period:		os << ".";				break;
	case Token::Colon:		os << ":";				break;
	case Token::SemiColon:	os << ";";				break;
	case Token::Assign:		os << ":=";				break;

	case Token::Round:		os << "Round";			break;
	case Token::Trunc:		os << "Trunc";			break;
	case Token::Abs:		os << "Abs";			break;
	case Token::Atan:		os << "ArcTan";			break;
	case Token::Exp:		os << "Exp";			break;
	case Token::Log:		os << "Ln";				break;
	case Token::Odd:		os << "Odd";			break;
	case Token::Pred:		os << "Pred";			break;
	case Token::Sin:		os << "Sin";			break;
	case Token::Sqr:		os << "Sqr";			break;
	case Token::Sqrt:		os << "Sqrt";			break;
	case Token::Succ:		os << "Succ";			break;
	case Token::Writeln:	os << "Writeln";		break;
	case Token::New:		os << "New";			break;
	case Token::Dispose:	os << "Dispose";		break;

	case Token::EOS:		os << "EOS";			break;

	default:
			os << "Unknown Kind: " << static_cast<unsigned>(kind) <<  "!";
	}

	return os;
}

