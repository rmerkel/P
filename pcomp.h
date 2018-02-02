/********************************************************************************************//**
 * @file pcomp.h
 *
 * The P compilier.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	PCOMP_H
#define	PCOMP_H

#include "compilier.h"

/********************************************************************************************//**
 * A P Compilier
 *
 * Productions for the P laugnuage recursive decent compilier.
 *
 * Evolved from https://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation.
 * Construction binds a program name with the instance, used in error messages. The compilier is
 * run via the call operator, specifing the input stream, the location of the emitted code, and
 * wheather to emit a travlelog (verbose messages).
 ************************************************************************************************/
class PComp : public Compilier {
public:
	PComp();								///< Constructor

private:
	bool isAnInteger(TDescPtr type);		///< Is type an integer?
	bool isAReal(TDescPtr type);			///< Is type a Real?

	/// Promote data type if necessary...
	TDescPtr promote(	TDescPtr				lhs,
						TDescPtr				rhs);

	/// Promote assigned data type if necessary...
	void assignPromote (TDescPtr				lhs,
						TDescPtr				rhs);

	/// array index production...
	TDescPtr varArray(	int						level,
						SymbolTable::iterator	it,
						TDescPtr				type);

	/// Record selection production...
	TDescPtr varSelector(SymbolTable::iterator	it,
						TDescPtr				type);

	/// variable sub-production...
	TDescPtr variable(	int						level,
						SymbolTable::iterator	it);

	TDescPtr builtInFunc(int level);		///< built-in functions

	/// factor-identifier sub-production...
	TDescPtr identFactor(int				level,
				const	std::string&		id);

	TDescPtr factor(int level);				///< factor production...
	TDescPtr term(int level);				///< terminal production...
	TDescPtr unary(int level);				///< unary-expr production...
	TDescPtr simpleExpr(int level);			///< simple-expr production...
	TDescPtr expression(int level);			///< expression production...
	TDescPtrVec	expressionList(int level);	///< expression-list production...
	std::pair<bool,Datum> constExpr();		///< const-expr production...

	/// call-statement production...
	void callStatement(	int					level,
						SymbolTable::iterator it);

	void whileStatement(int level);			///< while-statement production...
	void ifStatement(int level);			///< if-statement production...
	void repeateStatement(int level);		///< repeat-statement production...
 	void forStatement(int level);			///< for-statement production...

	/// assignment-statement production...
	void assignStatement(int				level,
						SymbolTable::iterator it,
						bool				dup = false);

	/// identifier-statement production...
	void identStatement(int					level,
				const	std::string&		id);

	void writeStmt(int level);				///< write/writeLn production..
	void writeStatement(int level);			///< write production..
	void writeLnStatement(int level);		///< writeln production..
	void statementNew(int level);			///< New statement production...
	void statementProcs(int level);			///< Built-in procedures productions...
	void statement(int level);				///< statement production...
	void statementList(int level);			///< statement-list-production...

	void constDeclList(int level);			///< const-declaration-list production...
	void constDecl(int level);				///< constant-declaration production...
	void typeDecl(int level);				///< type-declaracton production...
	void typeDeclList(int level);			///< type-declaraction-list production...
	int varDeclBlock(int level);			///< variable-declaration-block production...

	/// variable-declaration-list production...
	void varDeclList(	int					level,
						bool				params,
				const	std::string&		prefix,
						FieldVec&			idents);

	/// variable-declaration production...
	void varDecl(		int					level,
				const	std::string&		prefix,
				FieldVec&					idents);

	/// identifier-lst production...
	std::vector<std::string> identifierList(
						int					level,
				const	std::string&		prefix);

	/// type productions...
	TDescPtr type(		int					level,
				const	std::string&		prefix = "");

	TDescPtr simpleType(int level);			///< simple-type productions...
	TDescPtr ordinalType(int level);		///< ordinal type productions...

	/// structued-type productions...
	TDescPtr structuredType(int				level,
				const	std::string&		prefix);

	/// field-list productions...
	void fieldList(		int					level,
				const	std::string&		prefix,
						FieldVec&			idents);

	TDescPtrVec simpleTypeList(int level);	///< simple-type-list productions...

	/// Subroutine-declaration production...
	SymValue& subPrefixDecl(int				level,
							SymValue::Kind	kind);

	void procDecl(int level);				///< procedure-declaration production...
	void funcDecl(int level);				///< function-declaration production...
	void subDeclList(int level);			///< function/procedue declaraction productions...

	/// block-declaration production...
	unsigned blockDecl(	SymValue&			val,
						int					level);

	void progDecl(int level);				///< program-declaration production...

	void run() override;					///< run the compilier...
};

#endif
