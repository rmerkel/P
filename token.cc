/**	@file	token.cc
 *
 * TokenStream implementation. Started life as the Token and TokenStream classes from The C++
 * Programming Language, 4th Edition, by Stroustrup, and then modified to work on the integer based
 * P machine.
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

	case '<':									// <, <=, or <>?
		if (!getch(ch))		ct.kind = Token::LT;
		else if ('=' == ch)	ct.kind = Token::LTE;
		else if ('>' == ch) ct.kind = Token::NEQ;
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
	case '\'':									// ' c'  or ' string '
		ct.string_value = "";
		while (getch(ch) && ch != '\'')
			ct.string_value += ch;
		if (ch != '\'')
			ct.kind = Token::Unknown;
		else
			ct.kind = Token::String;
		return ct;

	default:							// ident, ident = or error
		if (isalpha(ch) || ch == '_') {
			ct.string_value = ch;
			while (getch(ch) && (isalnum(ch) || ch == '_'))
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
	{	"abs",			Token::Abs			},
	{	"and",			Token::And			},
	{	"array",		Token::Array		},
	{	"arctan",		Token::Atan			},
	{	"begin",		Token::Begin		},
	{	"bit_and",		Token::BitAnd		},
	{	"bit_not",		Token::BitNot		},
	{	"bit_or",		Token::BitOr		},
	{	"bit_xor",		Token::BitXor		},
	{	"boolean",		Token::BoolType		},
	{	"char",			Token::CharType		},
	{   "const",		Token::ConsDecl		},
	{	"dispose",		Token::Dispose		},
	{	"do",			Token::Do			},
	{	"else",			Token::Else			},
	{	"end",			Token::End			},
	{	"exp",			Token::Exp			},
	{	"function",		Token::FuncDecl		},
	{	"if",			Token::If			},
	{	"integer",		Token::IntType		},
	{	"ln",			Token::Log			},
	{	"mod",			Token::Mod			},
	{	"not",			Token::Not			},
	{	"new",			Token::New			},
	{	"odd",			Token::Odd			},
	{	"of",			Token::Of			},
	{	"ord",			Token::Ord			},
	{	"program",		Token::ProgDecl		},
	{	"procedure",	Token::ProcDecl		},
	{	"pred",			Token::Pred			},
	{	"record",		Token::Record		},
	{	"real",			Token::RealType		},
	{	"repeat",		Token::Repeat		},
	{	"round",		Token::Round		},
	{	"sin",			Token::Sin			},
	{	"sqr",			Token::Sqr			},
	{	"sqrt",			Token::Sqrt			},
	{	"succ",			Token::Succ			},
	{	"then",			Token::Then			},
	{	"trunc",		Token::Trunc		},
	{	"type",			Token::TypeDecl		},
	{	"until",		Token::Until		},
	{	"for",			Token::For			},
	{	"to",			Token::To			},
	{	"downto",		Token::DownTo,		},
	{	"var",			Token::VarDecl		},
	{	"while",		Token::While		},
	{	"write",		Token::Write		},
	{	"writeln",		Token::Writeln		}
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
	case Token::String:		os << "string";			break;
	case Token::IntegerNum:	os << "integernum";		break;
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

	case Token::BoolType:	os << "boolean";		break;
	case Token::CharType:	os << "char";			break;
	case Token::IntType:	os << "integer";		break;
	case Token::RealType:	os << "real";			break;

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

	case Token::BitNot:		os << "bit_not";		break;
	case Token::BitAnd:		os << "bit_and";		break;
	case Token::BitOr:		os << "bit_or";			break;
	case Token::BitXor:		os << "bit_xor";		break;

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

	case Token::Round:		os << "round";			break;
	case Token::Trunc:		os << "trunc";			break;
	case Token::Abs:		os << "abs";			break;
	case Token::Atan:		os << "arctan";			break;
	case Token::Exp:		os << "exp";			break;
	case Token::Log:		os << "ln";				break;
	case Token::Odd:		os << "odd";			break;
	case Token::Pred:		os << "pred";			break;
	case Token::Sin:		os << "sin";			break;
	case Token::Sqr:		os << "sqr";			break;
	case Token::Sqrt:		os << "sqrt";			break;
	case Token::Succ:		os << "succ";			break;
	case Token::Write:		os << "write";			break;
	case Token::Writeln:	os << "writeln";		break;
	case Token::New:		os << "new";			break;
	case Token::Dispose:	os << "dispose";		break;

	case Token::EOS:		os << "EOS";			break;

	default:
			os << "Unknown Kind: " << static_cast<unsigned>(kind) <<  "!";
	}

	return os;
}

