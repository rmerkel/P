/** @file pl0ccomp.h
 * 
 *  The PL/0C compiler.
 *  
 *  @section	compilier	The PL/0C compilier
 * 
 *  Started life as a C++ port of the PL/0 compilier as desicribed in Algorithms + Data Structures
 *  = Programs, 1st Edition, by Wirth and the C parser example from
 *  https://en.wikipedia.org/wiki/Recursive_descent_parser, and then modified to provide more "C
 *  like" operators.
 * 
 * 	@section grammer Grammer (EBNF)
 *
 *     prog  =     block "." ;
 *
 *     block =  [  "const" ident "=" number {"," ident "=" number} ";"]
 *              [  "var" ident {"," ident } ";"]
 *              {  "procedure" ident "(" [ ident { "," ident } ] ")" block ";"
 *               | "function"  ident "(" [ ident { "," ident } ] ")" block ";" }
 *                 stmt ;
 *
 *     stmt  = [   ident "=" expr
 *              |  ident "(" [ expr { "," expr } ")"
 *              |  "begin" stmt {";" stmt } "end"
 *              |  "if" cond "then" stmt { "else" stmt }
 *              |  "while" cond "do" stmt
 *              |  "repeat" stmt "until" cond ] ;
 *
 *     cond  =     relational { ("||" | &&") relation } ;
 * 
 *     relational= expr { ("==" | "!=" | "<" | "<=" | ">" | ">=") expr } ;
 * 
 *     expr =      [ ("+"|"-") ] add-expr { ("|" | "&" | "^") add-expr } ;
 * 
 *     add-expr = term { ("+" | "-") term } ;
 * 
 *     term =     fact { ("*" | "/" | "%") fact } ;
 * 
 *     fact  =    ident
 *  		   |  ident "(" [ ident { "," ident } ] ")" |  number |  "(" expr ")" ;
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

namespace pl0c {
	/// A PL/0C Compiler
	class Comp {
		/// A table, indexed by instruction address, yeilding source line numbers
		typedef std::vector<unsigned> SourceIndex;

		std::string			progName;			///< The compilier's name, used in error messages
		unsigned			nErrors;			///< Total # of compilier errors
		bool				verbose;			///< Dump debugging information if true
		TokenStream			ts;					///< The input token stream (the source)
		SymbolTable			symtbl;				///< Symbol table
		pl0c::InstrVector*	code;				///< Emitted code
		SourceIndex			indextbl;			///< Source cross-index for listings

	protected:
		void error(const std::string& msg);		///< Write an error message...

		/// Write an error message...
		void error(const std::string& msg, const std::string& name);

		Token next();							///< Read and return the next token...

		/// Return the current token kind..
		Token::Kind current() 				{	return ts.current().kind;	}

		/// Accept the next token..
		bool accept(Token::Kind k, bool get = true);

		/// Expect the next token...
		bool expect(Token::Kind k, bool get = true);

		/// Emit an instruction...
		size_t emit(const pl0c::OpCode op, int8_t level = 0, pl0c::Word addr = 0);

		/// Create a listing...
		void listing(const std::string& name, std::istream& source, std::ostream& out);

		void identifier(int level);				///< factor-identifier production...
		void unary(int level);					///< unary-expr production...
		void factor(int level);					///< factor production...
		void term(int level);					///< terminal production...
		void addExpr(int level);				///< additive-expr production...
		void expression(int level);				///< expression production...
		void relational(int level);				///< relational-expr production...
		void condition(int level);				///< condition production...

		/// assignment-statement production...
		void assignStmt(const std::string& name, const SymValue& val, int level);

		/// call-statement production...
		void callStmt(const std::string& name, const SymValue& val, int level);

		void identStmt(int level);				///< identifier-statement production...
		void whileStmt(int level);				///< while-statement production...
		void repeatStmt(int level);				///< repeat-statement production...
		void ifStmt(int level);					///< if-statement production...
		void statement(int level);				///< statement production...

		void constDecl(int level);				/// constant-declaration production...
		int  varDecl(int offset, int level);	/// variable-declaration production...
		void subDecl(int level);				/// subroutine-declaration production...

		/// block production...
		void block(SymValue& val, int level, unsigned nargs);

		void run();								///< runs the compilier...

	public:
		Comp(const std::string& pName);	///< Constructor; use pName for error messages
		virtual ~Comp() {}				///< Destructor

		/// Run the compiler
		unsigned operator()(const std::string& inFile, pl0c::InstrVector& prog, bool verb = false);
	};
};

#endif
