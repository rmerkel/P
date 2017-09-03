/********************************************************************************************//**
 * @file comp.h
 *
 * The Pascal-Lite compilier.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	COMP_H
#define	COMP_H

#include <set>
#include <string>
#include <utility>

#include "instr.h"
#include "datum.h"
#include "symbol.h"
#include "token.h"

/********************************************************************************************//**
 * A Pascal-Lite Compilier
 *
 * A recursive decent compilier, evolved from
 * https://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation. Construction binds
 * a program name with the instance, used in error messages. The compilier is run via the call
 * operator which specifies the input stream, the location of the emitted code, and weather to
 * emit a travlelog (verbose messages).
 ************************************************************************************************/
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

	// built-in types
	static	ConstTDescPtr	intDesc;		///< integer type descriptor
	static	ConstTDescPtr	realDesc;		///< Real type descriptor

	/// Name, kind pair
	struct NameKind {
		std::string		name;				///< Variable/parameter
		TDescPtr		type;				///< it's type descriptor

		/// Construct a name/kind pair
		NameKind(const std::string n, TDescPtr p) : name{n}, type{p} {}
	};

	/// A vector of name kind pairs
	typedef std::vector<NameKind>	NameKindVec;

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
	size_t emit(const OpCode op, int8_t level = 0, Datum addr = 0);

	/// Promote data type if necessary...
	ConstTDescPtr promote (ConstTDescPtr lhs, ConstTDescPtr rhs);

	/// Promote assigned data type if necessary...
	void assignPromote (ConstTDescPtr lhs, ConstTDescPtr rhs);

	/// Create a listing...
	void listing(const std::string& name, std::istream& source, std::ostream& out);

	/// Purge symtbl of entries from a given block level
	void purge(int level);

	/// Emit a variable reference, e.g., an absolute address...
	ConstTDescPtr emitVarRef(int level, const SymValue& val);

	SymbolTable::iterator identRef();		///< identifier sub-production...
	ConstTDescPtr identifier(int level);	///< factor-identifier production...
	ConstTDescPtr factor(int level);		///< factor production...
	ConstTDescPtr term(int level);			///< terminal production...
	ConstTDescPtr unary(int level);			///< unary-expr sub-production...
	ConstTDescPtr simpleExpr(int level);	///< simple-expr production...
	ConstTDescPtr expression(int level);	///< expression production...

	Datum constExpr();						///< const-expr production...

											/// assignment-statement production...
	void assignStmt(const std::string& name, const SymValue& val, int level);

											/// call-statement production...
	void callStmt(const std::string& name, const SymValue& val, int level);

	void identStmt(int level);				///< identifier-statement production...
	void whileStmt(int level);				///< while-statement production...
	void repeatStmt(int level);				///< repeat-statement production...
	void ifStmt(int level);					///< if-statement production...
	void statement(int level);				///< statement production...
	void statementList(int level);			///< statement-list-production...
	const std::string nameDecl(int level);	///< name (identifier) check...
	void constDeclList(int level);			///< const-declaration-list production...
	void constDecl(int level);				///< constant-declaration production...

	void typeDecl(int level);				///< type-declaracton production...
	void typeDeclList(int level);			///< type-declaraction-list production...

	int varDeclBlock(int level);			///< variable-declaration-block production...
											/// variable-declaration-list production...
	void varDeclList(int level, bool params, NameKindVec& idents);
											/// variable-declaration production...
	void varDecl(int level, NameKindVec& idents);
	ConstTDescPtr type();					///< type production...
											/// Subroutine-declaration production...
	SymValue& subPrefixDecl(int level, SymValue::Kind kind);
	void procDecl(int level);				///< procedure-declaration production...
	void funcDecl(int level);				///< function-declaration production...
	void subDeclList(int level);			///< function/procedue declaraction productions...
											/// block-declaration production...
	Unsigned blockDecl(SymValue& val, int level);

	void run();								///< runs the compilier...
};

#endif
