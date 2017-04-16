/** @file pl0ccomp.h
 *
 * The PL/0C compiler.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef	PL0COM_H
#define	PL0COM_H

#include <string>
#include <utility>

#include "pl0c.h"
#include "token.h"
#include "symbol.h"

namespace pl0c {
	/** A PL/0C Compilier
	 *
	 * A recursive decent compilier evolved from
	 * https://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation. Construction binds a
	 * program name with the instance, used in error messages. The compilier is run via the call
	 * operator which specifies the input stream, the location of the emitted code, and weather to emit
	 * a travlelog (verbose messages).
	 *
	 * @section grammer Grammer (EBNF)
	 *
	 *     program =        block-decl "." ;
	 *
	 *     block-decl =     [ "const" const-decl { "," const-decl } ";" ]
	 *  					[ "var" var-decl-list ";" ]
	 *  					{ proc-decl | funct-decl }
	 *  					stmt ;
	 *
	 *     const-decl =     ident "=" ( number | ident } ;
	 *
	 *     proc-decl =      "procedure" ident param-decl-list          block-decl ";" ;
	 *
	 *     func-decl =      "function"  ident param-decl-list ":" type block-decl ";" ;
	 *
	 *     param-decl-list= "(" [ var-decl-list ] ")" ;
	 *
	 *     var-decl-list =  var-decl { "," var-decl } ;
	 *
	 *     var-decl =       ident { "," ident } ":" type ;
	 *
	 *     type =           "integer" | "real" ;
	 *
	 *     stmt =           [ ident "=" expr                        |
	 *                        ident "(" [ expr { "," expr } ")"     |
	 *                        "begin" stmt {";" stmt } "end"        |
	 *                        "if" cond "then" stmt { "else" stmt } |
	 *                        "while" cond "do" stmt                |
	 *                        "repeat" stmt "until" cond ] ;
	 *
	 *     cond =           relat { ("||" | &&") relation } ;
	 *
	 *     relat =          expr { ("==" | "!=" | "<" | "<=" | ">" | ">=") expr } ;
	 *
	 *     expr =           shift-expr { ("|" | "&" | "^") shift-expr } ;
	 *
	 *     shift-expr =     add-expr { ("<<" | ">>") add-expr } ;
	 *
	 *     add-expr =       term { ("+" | "-") term } ;
	 *
	 *     term =           unary { ("*" | "/" | "%") unary } ;
	 *
	 *     unary =          [ ("+"|"-") ] fact ;
	 *
	 *     fact  =          ident                                   |
	 *                      ident "(" [ ident { "," ident } ] ")"   |
	 *                      number                                  |
	 *                      "(" expr ")"
	 *                      ;
	 *
	 * Key
	 * - {}	Repeat zero or more times
	 * - []	Optional; zero or *one* times
	 * - () Grouping
	 * - |  One of ...
	 * - ;  End of production
	 */
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
		size_t emit(const pl0c::OpCode op, int8_t level = 0, pl0c::Datum addr = pl0c::Integer{0});

		/// Create a listing...
		void listing(const std::string& name, std::istream& source, std::ostream& out);

		/// Emit a variable reference, e.g., an absolute address
		void varRef(int level, const SymValue& val);

		void identifier(int level);				///< factor-identifier production...
		void unary(int level);					///< unary-expr production...
		void factor(int level);					///< factor production...
		void term(int level);					///< terminal production...
		void addExpr(int level);				///< additive-expr production...
		void shiftExpr(int level);				///< shift production...
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

		const std::string nameDecl(int level);		///< name (identifier) check...

		void constDecl(int level);				///< constant-declaration production...
		int  varDecl(int offset, int level);	///< variable-declaration production...

		///< Subroutine-declaration production...
		SymValue& subroutineDecl(int level, SymValue::Kind kind);

		void procDecl(int level);				///< procedure-declaration production...
		void funcDecl(int level);				///< function-declaration production...

		/// block-declaration production...
		void blockDecl(SymValue& val, int level);

		void run();								///< runs the compilier...

	public:
		Comp(const std::string& pName);	///< Constructor; use pName for error messages
		virtual ~Comp() {}				///< Destructor

		/// Run the compiler
		unsigned operator()(const std::string& inFile, pl0c::InstrVector& prog, bool verb = false);
	};
};

#endif
