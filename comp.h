/** @file comp.h
 *
 * The PL/0C compiler.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef	COMP_H
#define	COMP_H

#include <set>
#include <string>
#include <utility>

#include "instr.h"
#include "datum.h"
#include "token.h"
#include "symbol.h"

/** A PL/0C Compilier
 *
 * A recursive decent compilier, evolved from
 * https://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation. Construction binds
 * a program name with the instance, used in error messages. The compilier is run via the call
 * operator which specifies the input stream, the location of the emitted code, and weather to
 * emit a travlelog (verbose messages).
 *
 * @section grammer Grammer (EBNF)
 *     
 *     program =        block-decl "." ;
 *     block-decl =     [ "const" const-decl-lst { ";" const-decl-lst ";" ]
 *                      [ "var" ident-list { ";" ident-lst } ";" ]
 *                      [ sub-decl { ";" sub-decl } ";" ]
 *                      stmt-blk 
 *                      ;
 *     
 *     const-decl-lst = const-decl { "," const-decl } ;
 *     const-decl =     ident "=" number | ident ;
 *
 *     sub-decl =       sub-type ident param-decl-lst block-decl ";" ;
 *     sub-type =       "procedure" | "function" ;
 *     param-decl-lst = "(" [ ident-lst ] ")" ;
 *     ident-lst =        ident { "," ident } ;
 *  
 *     stmt-blk =       "begin" stmt {";" stmt } "end" ;
 *     stmt =           [ ident "=" expr                      	|
 *                        ident "(" [ expr { "," expr } ")"     |
 *                        stmt-blk                              |
 *                        "if" expr "then" stmt { "else" stmt } |
 *                        "while" expr "do" stmt                |
 *  					  "repeat" expr "until" expr ]
 *     					;
 *     
 *     expr =			simple-expr {  relation-op simple-expr };
 *     relation-op =	"<"|"<="|"=="|"!="|">="|">";
 *     simple-expr =	[ unary-oper ] term { addr-oper term };
 *     unary_oper = 	"+" | "-" | "!" | "~"
 *     addr-oper =		"+" | "-" | "^" | "|" | "||";
 *     term = 			fact { multi-oper ) fact } ;
 *     multi-oper =		"*" | "/" | "%" | "&" [ "&&";
 *     fact  =          number									[
 *     					ident                                   |
 *                      ident "(" [ expr { "," expr } ")"       |
 *                      "round" "(" expr ")"                    |
 *  					"(" expr ")"
 *  					;
 *      
 * Key
 * - {}	Repeat zero or more times
 * - []	Optional; zero or *one* times
 * - () Grouping
 * - |  One of ...
 * - ;  End of production
 */
class Comp {
public:
	Comp(const std::string& pName);			///< Constructor; use pName for error messages
	virtual ~Comp() {}						///< Destructor

	/// Run the compiler
	unsigned operator()(const std::string& inFile, InstrVector& prog, bool verb = false);

private:
	/// A table, indexed by instruction address, yeilding source line numbers
	typedef std::vector<unsigned> SourceIndex;


	std::string			progName;			///< The compilier's name, used in error messages
	unsigned			nErrors;			///< Total # of compilier errors
	bool				verbose;			///< Dump debugging information if true
	TokenStream			ts;					///< The input token stream (the source)
	SymbolTable			symtbl;				///< Symbol table
	InstrVector*		code;				///< Emitted code
	SourceIndex			indextbl;			///< Source cross-index for listings

protected:
	///< A vector of identifiers
	typedef std::vector<std::string>	IdentVec;

	void error(const std::string& msg);		///< Write an error message...

	/// Write an error message...
	void error(const std::string& msg, const std::string& name);

	Token next();							///< Read and return the next token...

	/// Return the current token kind..
	Token::Kind current() 					{	return ts.current().kind;	}

	/// Accept the next token if it's a k...
	bool accept(Token::Kind k, bool get = true);

	/// Expect the next token to be a k...
	bool expect(Token::Kind k, bool get = true);

	bool oneOf(Token::KindSet set);			///< Is the current token one of the given set?

	/// Emit an instruction...
	size_t emit(OpCode op, int8_t level = 0, Datum addr = Datum());

	/// Return emit(op, level, Datum(value)
	template <class T> size_t emit(OpCode op, int8_t level, const T& value) {
		return emit(op, level, Datum(value));
	}

	/// Create a listing...
	void listing(const std::string& name, std::istream& source, std::ostream& out);

	/// Purge symtbl of entries from a given block level
	void purge(int level);

	/// Emit a variable reference, e.g., an absolute address...
	void varRef(int level, const SymValue& val);

	void identifier(int level);				///< factor-identifier production...
	void factor(int level);					///< factor production...
	void unary(int level);					///< unary-expr production...
	void term(int level);					///< terminal production...
	void simpleExpr(int level);				///< simple expression production...
	void expression(int level);				///< expression production...

	/// assignment-statement production...
	void assignStmt(const std::string& name, const SymValue& val, int level);

	/// call-statement production...
	void callStmt(const std::string& name, const SymValue& val, int level);

	void identStmt(int level);				///< identifier-statement production...
	void whileStmt(int level);				///< while-statement production...
	void repeatStmt(int level);				///< repeat-statement production...
	void ifStmt(int level);					///< if-statement production...
	void statement(int level);				///< statement production...
	void statementListTail(int level);		///< partial statement-list-production...

	const std::string nameDecl(int level);	///< name (identifier) check...

	void constDeclBlock(int level);			///< const-declaration-block production...
	void constDeclList(int level);			///< const-declaration-list production...
	void constDecl(int level);				///< constant-declaration production...

	int varDeclBlock(int level);			///< variable-declaration-block production...

	/// variable-declaration-list production...
	void varDeclList(int level, bool params, IdentVec& idents);

	/// ariable-declaration production...
	void varDecl(int level, IdentVec& idents);

	/// Subroutine-declaration production...
	SymValue& subPrefixDecl(int level, SymValue::Kind kind);

	void procDecl(int level);				///< procedure-declaration production...
	void funcDecl(int level);				///< function-declaration production...
	void subrountineDecls(int level);		///< function/procedue declaraction productions...

	/// block-declaration production...
	Datum::Unsigned blockDecl(SymValue& val, int level);

	void run();								///< runs the compilier...
};

#endif
