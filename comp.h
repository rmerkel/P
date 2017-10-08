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

#include "compbase.h"

/********************************************************************************************//**
 * A Pascal-Lite Compilier
 *
 * A recursive decent compilier, evolved from
 * https://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation. Construction binds
 * a program name with the instance, used in error messages. The compilier is run via the call
 * operator which specifies the input stream, the location of the emitted code, and weather to
 * emit a travlelog (verbose messages).
 ************************************************************************************************/
class Comp : public CompBase {
public:
	Comp(const std::string& pName);			///< Constructor; use pName for error messages

private:
	bool isAnInteger(TDescPtr type);		///< Is type an integer?
	bool isAReal(TDescPtr type);			///< Is type a Real?

	/// Promote data type if necessary...
	TDescPtr promote (TDescPtr lhs, TDescPtr rhs);

	/// Promote assigned data type if necessary...
	void assignPromote (TDescPtr lhs, TDescPtr rhs);

	/// variable sub-production...
	TDescPtr variable(int level, SymbolTable::iterator it);

	/// factor-identifier sub-production...
	TDescPtr identFactor(int level, const std::string& id);

	TDescPtr factor(int level);				///< factor production...
	TDescPtr term(int level);				///< terminal production...
	TDescPtr unary(int level);				///< unary-expr production...
	TDescPtr simpleExpr(int level);			///< simple-expr production...
	TDescPtr expression(int level);			///< expression production...
	TDescPtrVec	expressionList(int level);	///< expression-list production...
	Datum constExpr();						///< const-expr production...

	/// call-statement production...
	void callStmt(const std::string& name, const SymValue& val, int level);

	void whileStmt(int level);				///< while-statement production...
	void repeatStmt(int level);				///< repeat-statement production...
	void ifStmt(int level);					///< if-statement production...

	/// statement-identifier sub-production...
	void identStatement(int level, const std::string& id);

	void statement(int level);				///< statement production...
	void statementList(int level);			///< statement-list-production...
	void constDeclList(int level);			///< const-declaration-list production...
	void constDecl(int level);				///< constant-declaration production...

	void typeDecl(int level);				///< type-declaracton production...
	void typeDeclList(int level);			///< type-declaraction-list production...

	int varDeclBlock(int level);			///< variable-declaration-block production...
											/// variable-declaration-list production...
	void varDeclList(int level, bool params, NameKindVec& idents);

	/// variable-declaration production...
	void varDecl(int level, NameKindVec& idents);

	/// identifier-lst production...
	std::vector<std::string> identifierList(int level);

	TDescPtr type(int level);				///< type production...
	TDescPtr simpleType(int level);			///< simple-type production...
	TDescPtrVec simpleTypeList(int level);	///< simple-type-list productions...

	/// Subroutine-declaration production...
	SymValue& subPrefixDecl(int level, SymValue::Kind kind);
	void procDecl(int level);				///< procedure-declaration production...
	void funcDecl(int level);				///< function-declaration production...
	void subDeclList(int level);			///< function/procedue declaraction productions...

	/// block-declaration production...
	unsigned blockDecl(SymValue& val, int level);

	virtual void run();						///< run the compilier...
};

#endif
