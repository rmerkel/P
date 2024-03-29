/********************************************************************************************//**
 * @file comp.h
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
	size_t emitJump(size_t where = 0);		///< Emit a JUMP instruction...
	size_t emitJumpI(size_t where = 0);		///< Emit a JUMPI instruction...
	size_t emitJNEQ(size_t where = 0);		///< Emit a JNEQ instruction...
	size_t emitJNEQI(size_t where = 0);		///< Emit a JNEQI instruction...

	/// Emit a CALL insruction...
	size_t emitCall(int8_t level, size_t where);

	/// Emit a CALLI insruction...
	size_t emitCallI(int8_t level, size_t where);

	/// Promote data type if necessary...
	TDescPtr promote(TDescPtr lhs, TDescPtr rhs);

	/// Promote assigned data type if necessary...
	void assignPromote (TDescPtr lhs, TDescPtr rhs);

	/// array index production...
	TDescPtr varArray(	int					level,
						SymbolTableIter		it,
						TDescPtr			type);

	/// Attribute production...
	TDescPtr attribute(	SymbolTableIter	it,
						TDescPtr			type);

	/// Record selection production...
	TDescPtr varSelector(SymbolTableIter	it,
						TDescPtr			type);

	/// variable sub-production...
	TDescPtr variable(	int					level,
						SymbolTableIter		it);

	TDescPtr builtInFunc(int level);		///< built-in functions

	/// factor-identifier sub-production...
	TDescPtr identFactor(int				level,
				const	std::string&		id,
						bool				var);

	TDescPtr factor(int level, bool var);	///< factor production...
	TDescPtr term(int level, bool var);		///< terminal production...
	TDescPtr unary(int level, bool var);	///< unary-expr production...

	/// simple-expr production...
	TDescPtr simpleExpr(int level, bool var);

	/// expression production...
	TDescPtr expression(int level, bool var = false);

	TDescPtrVec	expressionList(int level);	///< expression-list production...

	/// A constant expression value. Second is valid if first is true
	typedef std::pair<bool,Datum> ConstExprValue;

	/// Promote lhs or rhs, as necessary...
	void constPromote(Datum& lhs, Datum& rhs);

	ConstExprValue constFactor();				///< const-factor productkion...
	ConstExprValue constTerm();				///< const-term production...
	ConstExprValue constUnary();			///< const-unary production...
	ConstExprValue constSimpleExpr(); 		///< const-simple-expr production...
	ConstExprValue constExpr();				///< const-expr production...

	/// call-statement production...
	void callStatement(	int					level,
						SymbolTableIter		it);


	/// Evaluate and emit a l-value reference...
	TDescPtr lvalueRef(int level, SymbolTableIter it, bool dup);

	/// assignment-statement production...
	void assignStatement(	int				level,
							SymbolTableIter	it,
							bool			dup = false);

	bool identStatement(int level);			///< identifier-statement production...

	/// if-statement production...
	bool ifStatement(int level, SymbolTableEntry& context);

	/// while-statement production...
	bool whileStatement(int level, SymbolTableEntry& context);

	/// repeat-statement production...
	bool repeatStatement(int level, SymbolTableEntry& context);

 	/// for-statement production...
 	bool forStatement(int level, SymbolTableEntry& context);

	/// return-statement production...
	bool returnStatement(int level, SymbolTableEntry& context);
					
	void getStatement(int level);			///< get production..
	void put(int level);					///< Prefix for put and putln productions...
	void putStatement(int level);			///< put production..
	void putLnStatement(int level);			///< putln production..
	void statementNew(int level);			///< New statement production...
	void statementProcs(int level);			///< Built-in procedures productions...

	/// statement productions...
	void statement(int level, SymbolTableEntry& constant);

	/// Statement-list production...
	void statementList(int level, SymbolTableEntry& constant);

	void constDeclList(int level);			///< const-declaration-list production...
	void constDecl(int level);				///< constant-declaration production...
	void typeDecl(int level, bool var);		///< type-declaracton production...
	void typeDeclList(int level);			///< type-declaraction-list production...
	int varDeclBlock(int level);			///< variable-declaration-block production...

	/// variable-declaration-list production...
	void varDeclList(	int					level,
						bool				params,
				const	std::string&		idprefix,
						FieldVec&			idents);

	/// variable-declaration production...
	void varDecl(		int					level,
						bool				var,
				const	std::string&		idprefix,
						FieldVec&			idents);

	/// identifier-lst production...
	std::vector<std::string> identifierList(
						int					level,
				const	std::string&		idprefix);

	/// type productions...
	TDescPtr type(		int 				level,
						bool				var,
				const	std::string&		idprefix = "");

	/// simple-type productions...
	TDescPtr simpleType(int level, bool var);

	/// ordinal-type-list productions...
	TDescPtrVec ordinalTypeList(int level, bool);

	/// ordinal type productions...
	TDescPtr ordinalType(int level, bool var);

	TDescPtr subRangeType(bool var);		///< sub-range type productions...

	/// structued-type productions...
	TDescPtr structuredType(int				level,
				const		std::string&	idprefix,
							bool			var);

	/// field-list productions...
	void fieldList(		int					level,
				const	std::string&		idprefix,
						FieldVec&			idents);

	/// variable-declaration-list production...
	void paramDeclList(	int					level,
						bool				params,
				const	std::string& 		idprefix,
						FieldVec&			idents);

	/// Subroutine-declaration production...
	SymbolTableIter subroutineDecl(
						int					level,
						SymValue::Kind		kind);

	void procDecl(int level);				///< procedure-declaration production...
	void funcDecl(int level);				///< function-declaration production...
	void subDeclList(int level);			///< function/procedue declaraction productions...

	/// block-declaration production...
	size_t blockDecl(SymbolTableEntry&		context,
					int 					level,
					Token::Kind				end);

	void progDecl();						///< program-declaration production...

	void run() override;					///< run the compilier...
};

#endif
