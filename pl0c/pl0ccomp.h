/** @file pl0ccomp.h
 *
 * A PL/0 compiler...
 *
 * Grammar (EBNF)
 * --------------
 * prog =	block "." ;
 * 
 * block = 	[ "const" ident "=" number {"," ident "=" number} ";"]
 *         	[ "var" ident {"," ident} ";"]
 *         	{ "procedure" ident "()" block ";" }
 *          stmt ;
 * 
 * stmt = 	[ ident ":=" expr
 * 		  	| "call" ident 
 *          | "?" ident 
 * 		  	| "!" expr 
 *          | "begin" stmt {";" stmt } "end" 
 *          | "if" cond "then" stmt { "else" stmt }
 *  		| "while" cond "do" stmt ]
 *  		| "repeat" stmt "until" cond ;
 * 
 * cond = 	  "odd" expr
 * 		  	| expr ("="|"!="|"<"|"<="|">"|">=") expr ;
 * 
 * expr = 	[ "+"|"-"] term { ("+"|"-") term } ;
 * 
 * term = 	fact {("*"|"/") fact} ;
 * 
 * fact = 	  ident
 *			| number
 *			| "(" expr ")" ;
 * 
 * Key
 * 	- {}	zero or more times
 * 	- []	zero or one times
 */

#ifndef	PL0COM_H
#define	PL0COM_H

#include "pl0c.h"
#include "token.h"
#include "symbol.h"

#include <string>

/// A PL/0C Compiler
class PL0CComp {
	std::string			progName;		///< The owning programs name
	unsigned			nErrors;		///< # of errors compiling all sources
	bool				verbose;		///< Dump debugging information if true
	TokenStream			ts;				///< Input token stream
	SymbolTable			symtbl;			///< The symbol table
	pl0c::InstrVector*	code;			///< The emitted code

protected:
	void error(const std::string& s);
	void error(const std::string& s, const std::string& t);
	Token next();

	/// Return the current token kind
	Token::Kind current() 				{	return ts.current().kind;	}
	bool accept(Token::Kind k, bool get = true);
	bool expect(Token::Kind k, bool get = true);

	size_t emit(const pl0c::OpCode op, int8_t level = 0, pl0c::Word addr = 0);
	void identifier(unsigned level);
	void factor(unsigned level);
	void terminal(unsigned level);
	void expression(unsigned level);
	void condition(unsigned level);
	void assignStmt(unsigned level);
	void callStmt(unsigned level);
	void whileStmt(unsigned level);
	void repeatStmt(unsigned level);
 	void ifStmt(unsigned level);
	void statement(unsigned level);
	void constDecl(unsigned level);
	int varDecl(int offset, unsigned level);
	void procDecl(unsigned level);
	void block(SymbolTable::iterator it, unsigned level);
	void run();

public:
	PL0CComp(const std::string& pName);
	virtual ~PL0CComp() {}

	unsigned operator()(const std::string& inFile, pl0c::InstrVector& code, bool verbose = false);
};

#endif
