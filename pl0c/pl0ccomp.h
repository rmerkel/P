/** @file pl0ccomp.h
 *
 * A PL/0C compiler...
 *
 * Grammar (EBNF):
 * --------------
 *
 *     prog  =   block "." ;
 *
 *     block = [ "const" ident "=" number {"," ident "=" number} ";"]
 *             [ "var" ident {"," ident } ";"]
 *             { "procedure" ident "(" [ ident { "," ident } ] ")" block ";"
 *              | "function" ident "(" [ ident { "," ident } } ")" block ";" }
 *               stmt ;
 *
 *     stmt  = [ ident "=" expr
 *              |ident "(" [ expr { "," expr } ")"
 *              |"begin" stmt {";" stmt } "end"
 *              |"if" cond "then" stmt { "else" stmt }
 *              |"while" cond "do" stmt
 *              |"repeat" stmt "until" cond ] ;
 *
 *     cond  =   "odd" expr
 *             | expr ("=="|"!="|"<"|"<="|">"|">=") expr ;
 *
 *     expr  = [ "+"|"-"] term { ("+"|"-") term };
 *
 *     term  =   fact {("*"|"/") fact} ;
 *
 *     fact  =   ident
 *             | ident "(" [ ident { "," ident } ] ")"
 *             | number
 *             | "(" expr ")" ;
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
	/// Write a error message
	void error(const std::string& msg);

	/** Write a error message
	 */
	void error(const std::string& msg, const std::string& name);

 	/// Read and returns the next token from the current token stream.
	Token next();

	/// Return the current token kind
	Token::Kind current() 				{	return ts.current().kind;	}

	/// Accept the next token
	bool accept(Token::Kind k, bool get = true);

	/// Expect teh next token
	bool expect(Token::Kind k, bool get = true);

	size_t emit(const pl0c::OpCode op, int8_t level = 0, pl0c::Word addr = 0);
	void identifier(int level);
	void factor(int level);
	void terminal(int level);
	void expression(int level);
	void condition(int level);
	void assignStmt(const std::string& name, const SymValue& val, int level);
	void callStmt(const std::string& name, const SymValue& val, int level);
	void identStmt(int level);
	void whileStmt(int level);
	void repeatStmt(int level);
 	void ifStmt(int level);
	void statement(int level);
	void constDecl(int level);
	int  varDecl(int offset, int level);
	void subDecl(int level);
	void block(SymValue& val, int level, unsigned nargs);
	void run();

public:
	PL0CComp(const std::string& pName);	///< Constructor; use pName for error messages
	virtual ~PL0CComp() {}				///< Destructor

	/// Run the compiler
	unsigned operator()(const std::string& inFile, pl0c::InstrVector& prog, bool verb = false);
};

#endif
