/********************************************************************************************//**
 * @file pascomp.cc
 *
 * Pascal-lite Compiler implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include "pascomp.h"
#include "interp.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

/************************************************************************************************
 * class PasComp
 ************************************************************************************************/

// private:

/********************************************************************************************//**
 * @param	type	Type descriptor to investagate
 * @return  true if type can be treated as an Integer
 ************************************************************************************************/
bool PasComp::isAnInteger(TDescPtr type) {
	return type->kind() == TDesc::Integer;
}

/********************************************************************************************//**
 * @param	type	Type descriptor to investagate
 * @return  true if type can be treated as an Real
 ************************************************************************************************/
bool PasComp::isAReal(TDescPtr type) {
	return	type->kind() == TDesc::Real;
}


/********************************************************************************************//**
 * Promote binary stack operands as necessary. 
 *
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 *
 * @return	Type type of the promoted type
 ************************************************************************************************/
TDescPtr PasComp::promote (TDescPtr lhs, TDescPtr rhs) {
	TDescPtr type = lhs;					// Assume that lhs and rhs have the same types
	if (lhs->kind() == rhs->kind())
		;									// nothing to do

	else if ((isAnInteger(lhs) && isAnInteger(rhs)) || (isAReal(lhs) && isAReal(rhs)))
		;									// nothing to do, again

	else if (isAnInteger(lhs) && isAReal(rhs)) {
		emit(OpCode::ITOR2);				// promote lhs (TOS-1) to a real
		type = rhs;

	} else if (isAReal(lhs) && isAnInteger(rhs))
		emit(OpCode::ITOR);					// promote rhs to a real
	
	else
		error("incompatable binary types");

	return type;
}

/********************************************************************************************//**
 * Convert rhs of assign to real if necessary, or emit error would need to be
 * converted to an integer. 
 *
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 ************************************************************************************************/
void PasComp::assignPromote (TDescPtr lhs, TDescPtr rhs) {
	if (lhs->kind() == rhs->kind())
		;				// nothing to do

	else if ((isAnInteger(lhs) && isAnInteger(rhs)) || (isAReal(lhs) && isAReal(rhs)))
		;				// nothing to do, again

	else if (isAnInteger(lhs) && isAReal(rhs)) {
		error("rounding lhs to fit in an integer");
		emit(OpCode::RTOI);					// promote rhs to a integer	 

	} else if (isAReal(lhs) && isAnInteger(rhs))
		emit(OpCode::ITOR);                 // promote rhs to a real

	else
		error("incompatable binary types");
}

/********************************************************************************************//**
 * Push a variable's value, a constant value, or invoke a function, and push the results,
 * of a function.
 *
 * ident | ident [ ( expr-lst ) ]
 *
 * @param	id		The variable or prodecure identifier
 * @param	level	The current block level 
 * @return	Data type 
 ************************************************************************************************/
TDescPtr PasComp::identFactor(int level, const string& id) {
	auto type = TDesc::intDesc;
   	auto it = lookup(id);

	if (it != symtbl.end()) {
		switch (it->second.kind()) {
		case SymValue::Constant:
			type = it->second.type();
			emit(OpCode::Push, 0, it->second.value());
			break;

		case SymValue::Variable:
			type = variable(level, it);
			emit(OpCode::Eval);
			break;

		case SymValue::Function:
			type = it->second.type();		// Use the function return type
			callStmt(it->first, it->second, level);
			break;

		default:
			error("Identifier is not a constant, variable or function", it->first);
		}
	}

	return type;
}

/********************************************************************************************//**
 * @param level		The current block level.
 *
 * @return	Data type 
 ************************************************************************************************/
TDescPtr PasComp::builtInFunc(int level)
{
	auto type = TDesc::intDesc;			// Factor data type
	ostringstream oss;

	if (accept(Token::Round))  {		// round(expr) to an integer
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() != TDesc::Real) {
			oss << "expeced real value, got: " << current();
			error(oss.str());

		} else {
			emit(OpCode::RTOI);
			type = TDesc::intDesc;
		}

	} else if (accept(Token::Trunc)) {	// truncate(expr) to an integer
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() != TDesc::Real) {
			oss << "expeced real value, got: " << current();
			error(oss.str());

		} else {
			emit(OpCode::TRUNC);
			type = TDesc::intDesc;
		}

	} else if (accept(Token::Abs)) {	// replace TOS with it's absolute value
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() != TDesc::Integer && type->kind() != TDesc::Real)
			oss << "expeced integer or real value, got: " << current();

		else
			emit(OpCode::ABS);

	} else if (accept(Token::Atan)) {	// replace TOS with it's arc tangent
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() == TDesc::Integer)
			type = TDesc::realDesc;		// Always produces a real
		else if (type->kind() != TDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::ATAN);

	} else if (accept(Token::Exp)) {	// Replace TOS with e raised to the given power TOS
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() == TDesc::Integer)
			type = TDesc::realDesc;		// Always produces a real
		else if (type->kind() != TDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::EXP);

	} else if (accept(Token::Log)) {	// Replace TOS with log(TOS)
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() == TDesc::Integer)
			type = TDesc::realDesc;		// Always produces a real
		else if (type->kind() != TDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::LOG);

	} else if (accept(Token::Odd)) {	// Replace TOS with is TOS odd?
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() != TDesc::Integer)
			oss << "expeced integer value, got: " << current();
		emit(OpCode::ODD);

	} else if (accept(Token::Ord)) {	// replace ordinal with it's ordinal
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (!type->isOrdinal()) {
			oss << "expected ordinal, got: " << current();
			error(oss.str());
		} else 
			type = TDesc::intDesc;

	} else {
		oss << "factor: syntax error; expected ident | num | { expr }, got: " << current();
		error(oss.str());
		next();
	}
		
	return type;
}

/********************************************************************************************//**
 * ident                                |
 * round ( expr )						|
 * ident [ ( expr-lst ) ]				|
 * number                               |
 * ( expr )
 * 
 * @param level		The current block level.
 *
 * @return	Data type 
 ************************************************************************************************/
TDescPtr PasComp::factor(int level) {
	auto type = TDesc::intDesc;			// Factor data type

	if (accept(Token::Identifier, false)) {
		// Copy, and then consume the identifer...
    	const string id = ts.current().string_value;
    	next();

		type = identFactor(level, id);

	} else if (accept(Token::IntegerNum, false)) {
		int value = ts.current().integer_value;
		emit(OpCode::Push, 0, value);
		expect(Token::IntegerNum);
		type = TDesc::intDesc;

	} else if (accept(Token::RealNum, false)) {
		emit(OpCode::Push, 0, ts.current().real_value);
		expect(Token::RealNum);
		type = TDesc::realDesc;

	} else if (accept(Token::OpenParen)) {
		type = expression(level);
		expect(Token::CloseParen);

	} else if (accept(Token::Not)) {
		emit(OpCode::LNOT);
		type = factor(level);

	} else 
		type = builtInFunc(level);

	return type;
}

/********************************************************************************************//**
 * fact { ( * | / | mod | and ) fact } ;
 *
 * @param level		The current block level 
 *
 * @return	Data type  
 ************************************************************************************************/
TDescPtr PasComp::term(int level) {
	auto lhs = factor(level);

	for (;;) {
		if (accept(Token::Multiply)) {
			lhs = promote(lhs, factor(level));
			emit(OpCode::Mul);
			
		} else if (accept(Token::Divide)) {
			lhs = promote(lhs, factor(level));
			emit(OpCode::Div);	
			
		} else if (accept(Token::Mod)) {
			lhs = promote(lhs, factor(level));
			emit(OpCode::Rem);

		} else if (accept(Token::And)) {
			lhs = promote(lhs, factor(level));
			emit(OpCode::LAND);

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * [ ( + | - ] term
 *
 * @param level		The current block level 
 *
 * @return	Data type 
 ************************************************************************************************/
TDescPtr PasComp::unary(int level) {
	auto type = TDesc::intDesc;				// Default factor data type

	if (accept(Token::Add)) 
		type = term(level);					// ignore unary + 

	else if (accept(Token::Subtract)) {
		type = term(level);
		emit(OpCode::Neg);

	} else									
		type = term(level);

	return type;
}

/********************************************************************************************//**
 * term { (+ | - | or ) term } ;
 *
 * @param level		The current block level. 
 *
 * @return	Data type  
 ************************************************************************************************/
TDescPtr PasComp::simpleExpr(int level) {
	auto lhs = unary(level);

	for (;;) {
		if (accept(Token::Add)) {
			lhs = promote(lhs, unary(level));
			emit(OpCode::Add);

		} else if (accept(Token::Subtract)) {
			lhs = promote(lhs, unary(level));
			emit(OpCode::Sub);

		} else if (accept(Token::Or)) {
			lhs = promote(lhs, unary(level));
			emit(OpCode::LOR);

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * simpleExpr { (< | <= | = | >= | > | <> ) simpleExpr } ;
 *
 * @param level		The current block level 
 *
 * @return	Data type. 
 ************************************************************************************************/
TDescPtr PasComp::expression(int level) {
	auto lhs = simpleExpr(level);

	for (;;) {
		if (accept(Token::LTE)) {
			lhs = promote(lhs, simpleExpr(level));
			emit(OpCode::LTE);

		} else if (accept(Token::LT)) {
			lhs = promote(lhs, simpleExpr(level));
			emit(OpCode::LT);

		} else if (accept(Token::GT)) {
			lhs = promote(lhs, simpleExpr(level));
			emit(OpCode::GT);
			
		} else if (accept(Token::GTE)) {
			lhs = promote(lhs, simpleExpr(level));
			emit(OpCode::GTE);
			
		} else if (accept(Token::EQU)) {
			lhs = promote(lhs, simpleExpr(level));
			emit(OpCode::EQU);

		} else if (accept(Token::NEQ)) {
			lhs = promote(lhs, simpleExpr(level));
			emit(OpCode::NEQU);

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * expr-lst: expression { ',' expression } ;
 *
 * @param level		The current block level 
 *
 * @return a list (vector) of TDescPtrs, one for each array dimension
 ************************************************************************************************/
TDescPtrVec	PasComp::expressionList(int level) {
	TDescPtrVec	v;

	do {
		v.push_back(expression(level));
	} while(accept(Token::Comma));

	return v;
}

/********************************************************************************************//**
 * [ + | - ] number | (const) identifer
 *
 * @return A boolean, constant value pair. Second is valid only if first is true.
 ************************************************************************************************/
pair<bool,Datum> PasComp::constExpr() {
	auto value = make_pair(true, Datum(0));
	int sign = 1;

	if (accept(Token::Add))
		;											// ignore unary + 
	else if (accept(Token::Subtract))
		sign = -1;

	if (accept(Token::IntegerNum, false)) {
		value.second = sign * ts.current().integer_value;
		next();										// Consume the number

	} else if (accept(Token::RealNum, false)) {
		value.second = sign * ts.current().real_value;
		next();										// Consume the number

	} else if (accept(Token::Identifier, false)) {
		// Copy, and then, consume the identifier..
		const string id = ts.current().string_value;
		expect(Token::Identifier);

		auto it = lookup(id);
		if (it != symtbl.end()) {
			if (it->second.kind() == SymValue::Constant)
				value.second = sign * it->second.value();
			else
				error("Identifier is not a constant, variable or function", it->first);
		}

	} else
		value.first = false;

	return value;
}

/********************************************************************************************//**
 * Call a function or procedure...
 *
 * ident [ '(' expr-list ')' ]...
 *
 * @param	name	The identifier value
 * @param	val		The identifiers symbol table entry value
 * @param	level	The current block level 
 ************************************************************************************************/
void PasComp::callStmt(const string& name, const SymValue& val, int level) {
	if (accept(Token::OpenParen)) {
		unsigned nParams = 0;					// Count actual parameters
		const auto& params = val.params();		// Formal parameter kinds
		if (!accept(Token::CloseParen, false))
			do {								// collect actual parameters
				const auto kind = expression(level);
				if (params.size() > nParams)
					promote(kind, params[nParams]);
				++nParams;

			} while (accept (Token::Comma));

		expect(Token::CloseParen);

		if (nParams != params.size()) {			// Is the caller passing right # of params?
			ostringstream oss;
			oss << "passing " << nParams << " parameters, where " << params.size() << " expected";
			error(oss.str());
		}
	}

	if (SymValue::Procedure != val.kind() && SymValue::Function != val.kind())
		error("Identifier is not a function or procedure", name);

	emit(OpCode::Call, level - val.level(), val.value());
}

/********************************************************************************************//**
 * while expr do statement...
 *
 * @param	level	The current block level.
 ************************************************************************************************/
 void PasComp::whileStmt(int level) {
	const auto cond_pc = code->size();	// Start of while expr
	expression(level);

	// jump if expr is false...
	const auto jmp_pc = emit(OpCode::JNEQ, 0, 0);
	expect(Token::Do);					// consume "do"
	statement(level);

	emit(OpCode::Jump, 0, cond_pc);		// Jump back to expr test...

	if (verbose)
		cout << progName << ": patching address at " << jmp_pc << " to " << code->size() << "\n";
	(*code)[jmp_pc].addr = code->size(); 
 }

/********************************************************************************************//**
 * if expr then statement [ else statement ]
 *
 * @param	level 	The current block level
 ************************************************************************************************/
 void PasComp::ifStmt(int level) {
	expression(level);

	// Jump if conditon is false
	const size_t jmp_pc = emit(OpCode::JNEQ, 0, 0);
	expect(Token::Then);							// Consume "then"
	statement(level);

	// Jump over else statement, but only if there is an else
	const bool Else = accept(Token::Else);
	size_t else_pc = 0;
	if (Else) else_pc = emit(OpCode::Jump, 0, 0);

	if (verbose)
		cout << progName << ": patching address at " << jmp_pc << " to " << code->size() << "\n";
	(*code)[jmp_pc].addr = code->size();

	if (Else) {
		statement(level);

		if (verbose)
			cout << progName << ": patching address at " << else_pc << " to " << code->size() << "\n";
		(*code)[else_pc].addr = code->size();
	}
 }

/********************************************************************************************//**
 * repeat statement until expr
 *
 * @param	level 	The current block level
 ************************************************************************************************/
 void PasComp::repeatStmt(int level) {
	 const size_t loop_pc = code->size();			// jump here until expr fails
	 statement(level);
	 expect(Token::Until);
	 expression(level);
	 emit(OpCode::JNEQ, 0, loop_pc);
 }

/********************************************************************************************//**
 * stmt { ; stmt }
 *
 * @param	level		The current block level.
 ************************************************************************************************/
void PasComp::statementList(int level) {
	do {
		statement(level);
	} while (accept(Token::SemiColon));
}

/********************************************************************************************//**
 * identifier [ '[' expression-list ']' { '[' expression-list /]' } ] 
 *
 * Emits a variable reference, optionally with array indexes.
 *
 * @param	id		The variable or array identifier
 * @param	level	The current block level.
 ************************************************************************************************/
TDescPtr PasComp::variable(int level, SymbolTable::iterator it) {
	auto type = it->second.type();			// Default is the symbol type

	emitVarRef(level, it->second);
	if (accept(Token::OpenBrkt)) {			// variable is an array, index into it
		do {
			auto atype = type;				// The arrays type, e.g, TDesc::Array
			type = atype->base();			// We'll return the arrays base type...

			if (atype->kind() != TDesc::Array)
				error("attempt to index into non-array", it->first);

			auto indexes = expressionList(level);
			if (indexes.empty())
				error("expected expression-list");

			unsigned nindexes = indexes.size();
			for (auto index : indexes) {
				if (atype->rtype()->kind() != index->kind()) {
					ostringstream oss;
					oss	<< "incompatable array index type, expected "
						<< atype->rtype()->kind() << " got " << index->kind();
					error(oss.str());
					
				} else if (type->size() != 1) {
					emit(OpCode::Push, 0, type->size());
					emit(OpCode::Mul);		// scale the index
				}

				// offset index for non-zero based arrays
				if (atype->range().minimum() != 0) {
					emit(OpCode::Push, 0, atype->range().minimum());
					emit(OpCode::Sub);
				}

				emit(OpCode::Add);			// index into the array
				
				if (--nindexes) {			// reditect if there's another index
					auto atype = type;		// The arrays type; kind s/b TDesc::Array
					type = atype->base();	// We'll return the arrays base type...
				}
			}

			expect(Token::CloseBrkt);

		} while (accept(Token::OpenBrkt));
	}

	return type;
}

/********************************************************************************************//**
 * variable := expression | identifier [ '(' expression-list ')' ]
 *
 * Emits code for an assignment statement, or a procedure call...
 *
 * @param	id		The variable or prodecure identifier
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::identStatement(int level, const string& id) {
	auto lhs = lookup(id);
	if (lhs == symtbl.end())
		return;							// id is unidentified

	auto type = lhs->second.type();		// Emit the lvalue...
	switch(lhs->second.kind()) {
	case SymValue::Function:			// emit reference to function return value
		emit(OpCode::PushVar, 0, FrameRetVal);
		break;

	case SymValue::Procedure:			// emit a procedure call
		callStmt(lhs->first, lhs->second, level);
		return;

	case SymValue::Variable:			// emit a reference to a variable, or array, value
		type = variable(level, lhs);
		break;

	case SymValue::Constant:
		error("Can't assign to a constant", lhs->first);
		break;

	default:
		error("expected variable, function return ref, or procedure call, got", lhs->first);
	}

	expect(Token::Assign);				// Emit the rvalue...
	const auto rhs = expression(level);

	switch(lhs->second.kind()) {		// Emit the assignment
	case SymValue::Variable:
		assignPromote(type, rhs);
		emit(OpCode::Assign);
		break;

	case SymValue::Function:			// Save value in the frame's retValue element
		assignPromote(type, rhs);
		emit(OpCode::Assign);
		break;

	default:
		assert(false);
	}
}

/********************************************************************************************//**
 * [ ident = expr						|
 *   if expr then stmt { else stmt }	|
 *   while expr do stmt					|
 *   repeat stmt until expr				|
 *   stmt-block ]
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::statement(int level) {
	if (accept(Token::Identifier, false)) {		// Assignment or procedure call
		// Copy and then consume the l-value identifier...
		const string id = ts.current().string_value;
		next();

		identStatement(level, id);

	} else if (accept(Token::Begin)) {				// begin ... end
		statementList(level);
		expect(Token::End);

	} else if (accept(Token::If)) 					// if expr...
		ifStmt(level);

	else if (accept(Token::While))					// "while" expr...
		whileStmt(level);

	else if (accept(Token::Repeat))					// "repeat" until...
		repeatStmt(level);

	// else: nothing
}

/********************************************************************************************//**
 * const const-decl { ; const-decl } ;
 *
 * @note	Doesn't emit any code; just stores the named value in the symbol table.
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::constDeclList(int level) {
	// Stops if the ';' if followd by any of hte following tokens
	static const Token::KindSet stops {
		Token::TypeDecl,
		Token::VarDecl,
		Token::ProcDecl,
		Token::FuncDecl,
		Token::Begin
	};

	if (accept(Token::ConsDecl)) {
		do {
			if (oneOf(stops))
				break;							// No more constants...
			constDecl(level);

		} while (accept(Token::SemiColon));
	}
}

/********************************************************************************************//**
 * ident = type
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::typeDecl(int level) {
	const auto ident = nameDecl(level);				// Copy the identifier
	expect(Token::EQU);								// Consume the "="
	auto tdesc = type(level);

	if (verbose)
		cout << progName << ": type " << ident << " = " << tdesc->kind()	<< "\n";

	symtbl.insert(	{ ident, SymValue(level, tdesc) }	);
}

/********************************************************************************************//**
 * type type-decl { ; type-decl } ;
 *
 * @note	Doesn't emit any code; just stores the new type in the symbol table.
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::typeDeclList(int level) {
	// Stops if the ';' if followd by any of hte following tokens
	static const Token::KindSet stops {
		Token::VarDecl,
		Token::ProcDecl,
		Token::FuncDecl,
		Token::Begin
	};

	if (accept(Token::TypeDecl)) {
		do {
			if (oneOf(stops))
				break;							// No more constants...
			typeDecl(level);

		} while (accept(Token::SemiColon));
	}
}

/********************************************************************************************//**
 * ident = const-expr ;
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::constDecl(int level) {
	auto ident = nameDecl(level);				// Copy the identifier
	expect(Token::EQU);							// Consume the "="
	auto value = constExpr();					// Get the constant value

	if (!value.first)
		error("expected a const-expression, got:", ts.current().string_value);

	TDescPtr type = value.second.kind() == Datum::Kind::Integer ? TDesc::intDesc : TDesc::realDesc;

#if 1
	symtbl.insert(	{ ident, SymValue::makeConst(level, value.second, type) } );
#else
	symtbl.insert(	{ ident, SymValue(level, value.second, type) }	);
#endif
	if (verbose)
		cout << progName << ": constDecl " << ident << ": " << level << ", " << value.second << "\n";
}

/********************************************************************************************//**
 * const" var-decl-lst ;
 *
 * @param	level	The current block level.
 * @return  Number of variables allocated before or after the activation frame.
 ************************************************************************************************/
int PasComp::varDeclBlock(int level) {
	NameKindVec	idents;							// vector of name/type pairs.

	if (accept(Token::VarDecl))
		varDeclList(level, false, idents);

	int sum = 0;								// Add up the size of every variable in the block
	for (const auto& id : idents)
		sum += id.type->size();

	return sum;
}

/********************************************************************************************//**
 * var-decl { ';' var-decl }
 *
 * Allocate space on the stack for each variable, as a postivie offset from the end of current
 * activaction frame. Create a new entry in the symbol table, that notes the offset and data
 * type.
 *
 * @param			level	The current block level.
 * @param			params	True if processing formal parameters, false if variable declaractions. 
 * @param[in,out]	idents	Vector of identifer, kind pairs 
 *
 * @return  Offset of the next variable/parmeter from the current activicaqtion frame.
 ************************************************************************************************/
void PasComp::varDeclList(int level, bool params, NameKindVec& idents) {
	// Stops if the ';' if followd by any of hte following tokens
	static const Token::KindSet stops {
		Token::ProcDecl,
		Token::FuncDecl,
		Token::Begin,
		Token::CloseParen
	};

	do {
		if (oneOf(stops))
			break;								// No more variables...
		varDecl(level, idents);
	} while (accept(Token::SemiColon));

	// Set the offset from the activation frame, parameters have negative offsets in reverse
	int dx = params ? 0 - idents.size() : 0;
	for (const auto& id : idents) {
		if (verbose)
			cout << progName  
				 << ": var/param " 	<< id.name << ": " 
				 << level 			<< ", "
			     << dx	 			<< ", " 
				 << id.type->kind() << "\n";

		auto range = symtbl.equal_range(id.name);		// Already defined?
		for (auto it = range.first; it != range.second; ++it) {
			if (it->second.level() == level) {
				error("previously was defined", id.name);
				continue;
			}
		}

#if 1
		symtbl.insert( { id.name, SymValue::makeVar(level, dx, id.type)	} );
#else
		symtbl.insert( { id.name, SymValue(level, dx, id.type)	} );
#endif
		dx += id.type->size();
	}
}

/********************************************************************************************//**
 * A semicolon list of variable, or formal parameter declractions. Each declaraction starts
 * with a comma separated list of identifiers, followed by a colon followed by a type
 * specifier:
 *
 *     var-decl =	    ident-list : type ;
 *     ident-list =     ident { "," ident } ;
 *
 * Allocate space on the stack for each variable, as a positive offset from the *end* of
 * current activaction frame; 0, 1, ..., n-1. Parameters, pushed by the caller, are identified
 * by negative indexes from *before the start* of the frame; -1, -2, ..., -n. Create a new
 * entry in the symbol table for either.
 *  
 * @param			level	The current block level. 
 * @param[in,out]	idents	Vector of identifer, kind pairs
 ************************************************************************************************/
void PasComp::varDecl(int level, NameKindVec& idents) {
	vector<string> ids = identifierList(level);
	expect(Token::Colon);
	const auto desc = type(level);
	for (auto& id : ids)
		idents.push_back({ id, desc });
}

/********************************************************************************************//**
 * identifier-lst : identifer { ',' identifier }
 * @param			level	The current block level. 
 * @return	List of identifiers in the identifier-lst
 ************************************************************************************************/
vector<string> PasComp::identifierList(int level) {
	vector<string> ids;

	do {
		ids.push_back(nameDecl(level));
	} while (accept(Token::Comma));

	return ids;
}

/********************************************************************************************//**
 * simple-type | structured-type | pointer-type ;
 *
 * Previously defined types, as well as  built-in types such as "Integer" and "Real" will have
 * Token::Type, thus we only need to look for new type declaractions, e.g., "array...".
 *
 * @param	level	The current block level. 
 * @return the type description
 ************************************************************************************************/
TDescPtr PasComp::type(int level) {
	auto tdesc = TDesc::intDesc;

	if (accept(Token::Identifier, false)) {		// previously defined type-name
		const string id = ts.current().string_value;
		next();

		auto it = lookup(id);
		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected type, got ", id);
		else
			tdesc = it->second.type();

	} else if ((tdesc = structuredType(level)) != 0) {
		;

	} else if ((tdesc = simpleType(level)))
		;

#if 0
	else if ((tdesc = pointerType(level)))
		;
#endif
	else
		tdesc = TDesc::intDesc;

	return tdesc;
}

/********************************************************************************************//**
 * integer' | '(' ident-list ')' | const-expr '..' const-expr
 *
 * @note Needs to handle previously defined simple-types
 * 
 * @param	level	The current block level. 
 * @return the type description
 ************************************************************************************************/
TDescPtr PasComp::simpleType(int level) {
	TDescPtr type;

	// Copy if next token is an identifier...
	if (accept(Token::Identifier, false)) {		// Previously defined type, must be ordinal!
		const string id = ts.current().string_value;
		next();

		auto it = lookup(id);
		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected type, got ", it->first);
		else if (!it->second.type()->isOrdinal())
			error("expected ordinal type, got ", it->first);
		else
			type = it->second.type();

	} else if (accept(Token::RealType))			// Real
		type = TDesc::realDesc;

	else 
		type = ordinalType(level);

	return type;
}

/********************************************************************************************//**
 * 'Boolean' | 'Integer' | 'Char' |
 * '(' identifier-list ')' | 'array' '[' simple-type-list ']' 'of' type ;
 *
 * @param	level	The current block level. 
 * @return type description if successful, null pointer otherwise
 ************************************************************************************************/
TDescPtr PasComp::ordinalType(int level) {
	TDescPtr type;

	if (accept(Token::IntType))					// Integer
		return TDesc::intDesc;

	else if (accept(Token::OpenParen)) {		// Enumeration
		FieldVec		enums;

		const auto ids = identifierList(level);
		SubRange r(0, ids.empty() ? 0 : ids.size());
		expect(Token::CloseParen);

		// Create the type, excluding the fields (enumerations)
		auto t = TDesc::newTDesc(TDesc::Enumeration, 1, r, TDescPtr(), TDesc::intDesc);

		unsigned value = 0;						// Each enumeration gets a value...
		for (auto id : ids) {
			enums.push_back( { id, TDesc::intDesc } );
#if 1
			symtbl.insert(	{ id, SymValue::makeConst(level, Datum(value), t) }	);
#else
			symtbl.insert(	{ id, SymValue(level, Datum(value), t) }	);
#endif
			if (verbose)
				cout << progName << ": enumeration '" << id << "' = " << value << ", " << level << "\n";
			++value;
		}

		t->fields(enums);
		type = t;

	} else {									// Sub-Range
		auto minValue = constExpr();
		if (minValue.first) {
			ostringstream oss;

			expect(Token::Ellipsis);
			auto maxValue = constExpr();
			if (!maxValue.first) {
				oss << "expected constant expression, got: " << current();
				error(oss.str());
				maxValue.second = minValue.second;
			}

			if (minValue.second > maxValue.second) {
				oss << "Minimum sub-range value (" << minValue.second << ")"
					<< " is greater than the maximum value (" << maxValue.second << ")";
				error(oss.str());
				swap(minValue.second, maxValue.second);

			} else if (	minValue.second.kind() != Datum::Kind::Integer		||
						maxValue.second.kind() != Datum::Kind::Integer) {
				oss << "Both sub-range values must be ordinal types; "
					<< minValue.second << ", " << maxValue.second;
				error(oss.str());
				minValue.second = 0; maxValue.second = 1;
			}

			SubRange r(minValue.second.integer(), maxValue.second.integer());

			type = TDesc::newTDesc(TDesc::Integer, 1, r);
		}
	}

	return type;
}

/********************************************************************************************//**
 * 'array' '[' simple-type-list ']' 'of' type | 'record' field-list 'end' ;
 *
 * @param	level	The current block level. 
 * @return type description if successful, null pointer otherwise
 ************************************************************************************************/
TDescPtr PasComp::structuredType(int level) {
	TDescPtr tdesc = 0;

	if (accept(Token::Array)) {					// Array
		expect(Token::OpenBrkt);				// "["

		TDescPtr tp = 0;
		TDescPtrVec indexes = simpleTypeList(level);
		for (auto index : indexes) {
			const SubRange r = index->range();
			if (tp == 0) {
				tp = TDesc::newTDesc(TDesc::Array, r.span(), r, index);
				tdesc = tp;						// Remember the first array...

			} else {							// Following indexes...
				tp->base(TDesc::newTDesc(TDesc::Array, r.span(), r, index));
				tp->size(tp->size() * r.span());
				tp = tp->base();
			}
		}

		expect(Token::CloseBrkt);				// "] of"
		expect(Token::Of);

		tp->base(type(level));					// Get the arrays base type
		tp->size(tp->size() * tp->base()->size());

#if 0
	} else if (accept(Token::Record)) {			// Record
		expect(Token::End);
#endif
	}

	return tdesc;
}

/********************************************************************************************//**
 * simple-type-lst: simple-type { ',' simple-type } ;
 *
 * @param	level	The current block level. 
 ************************************************************************************************/
TDescPtrVec PasComp::simpleTypeList(int level) {
	TDescPtrVec tdescs;

	do {
		tdescs.push_back(simpleType(level));
	} while (accept(Token::Comma));

	return tdescs;
}

/********************************************************************************************//**
 * Process the common subroutine delcaration for procedures and functions:
 *
 * ident [ ( var-decl-lst ) ] ) ...
 *
 * @param	level	The current block level.
 * @param 	kind	The type of subroutine, e.g., procedure or fuction
 * @return	subrountine's symbol table entry
 ************************************************************************************************/
SymValue& PasComp::subPrefixDecl(int level, SymValue::Kind kind) {
	SymbolTable::iterator	it;				// Will point to the new symbol table entry...

	auto ident = nameDecl(level);			// insert the name into the symbol table
	it = symtbl.insert( { ident, SymValue(kind, level)	} );
	if (verbose)
		cout << progName << ": subPrefixDecl " << ident << ": " << level << ", 0\n";

	// Process the formal auguments, if any...
	if (accept(Token::OpenParen)) {
		// Note that the activation frame level is that of the *following* block!

		NameKindVec	idents;					// vector of name/type pairs.
		varDeclList(level+1, true, idents);
		expect(Token::CloseParen);

		for (auto id : idents)
			it->second.params().push_back(id.type);
	}

	return it->second;
}

/********************************************************************************************//**
 * procedure ident [ ( var-decl-lst ) ] ; block ;
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::procDecl(int level) {
	auto& val = subPrefixDecl(level, SymValue::Procedure);
	expect(Token::SemiColon);
	blockDecl(val, level + 1);
	expect(Token::SemiColon);				// procedure declarations end with a ';'!
}

/********************************************************************************************//**
 * function ident [ ( var-decl-lst ) ] : type  block-decl ;
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PasComp::funcDecl(int level) {
	auto& val = subPrefixDecl(level, SymValue::Function);
	expect(Token::Colon);
	val.type(type(level));
	expect(Token::SemiColon);
	blockDecl(val, level + 1);
	expect(Token::SemiColon);	// function declarations end with a ';'!
}

/********************************************************************************************//**
 * proc-decl | func-decl
 *
 * Zero or more function and/or procedure delclaractions:
 *
 *     { proc-decl | funct-decl }
 *     proc-decl =      "procedure" ident param-decl-lst block-decl ";" ;
 *     func-decl =      "function"  ident param-decl-lst ":" type block-decl ";" ;
 *     type ; type =    "Integer" | "Real" ;
 *
 * @param level The current block level.
 ************************************************************************************************/
void PasComp::subDeclList(int level) {
	for (;;) {
		if (accept(Token::ProcDecl))
			procDecl(level);

		else if (accept(Token::FuncDecl))
			funcDecl(level);

		else
			break;
	}
}

/********************************************************************************************//**
 * [ const-decl-lst ]
 * [ var-decl-blk   ]
 * [ sub-decl-lst   ]
 * [ stmt-block     }
 *
 * @param	val		The blocks (procedures) symbol table entry value
 * @param	level	The current block level.
 * @return 	Entry point address
 ************************************************************************************************/
unsigned PasComp::blockDecl(SymValue& val, int level) {
	constDeclList(level);						// declaractions...
	typeDeclList(level);
	auto dx = varDeclBlock(level);
	subDeclList(level);

	/* Block body
	 *
	 * Emit the block's prefix, saving and return its address, followed by the postfix. Omit the prefix
	 * if dx == 0 (the subroutine has zero locals.
	 */

	const auto addr = dx > 0 ? emit(OpCode::Enter, 0, dx) : code->size();
	val.value(addr);

	if (expect(Token::Begin)) {					// "begin" statements... "end"
		statementList(level);
		expect(Token::End);
	}

	// block postfix... TBD; emit reti or retr for functions!

	const auto sz = val.params().size();
	if (SymValue::Function == val.kind())
		emit(OpCode::Retf, 0, sz);	// function...
	else
		emit(OpCode::Ret, 0, sz);	// procedure...

	purge(level);								// Remove symbols only visible at this level

	return addr;
}

/********************************************************************************************//**
 ************************************************************************************************/
void PasComp::run() {
	const int level = 0;						// Start at block level 0
	next();										// Fetch the 1st token

	expect(Token::ProgDecl);					// Program heading...
	auto& val = subPrefixDecl(level, SymValue::Procedure);
	expect(Token::SemiColon);

	// Emit a call to the main procedure, followed by a halt
	const auto call_pc = emit(OpCode::Call, level, 0);
	emit(OpCode::Halt);

	const auto addr = blockDecl(val, level);	// emit the first block 
	if (verbose)
		cout << progName << ": patching call to program at " << call_pc << " to " << addr  << "\n";

	(*code)[call_pc].addr = addr;

	expect(Token::Period);
}

/************************************************************************************************
 * public:
 ************************************************************************************************/

/********************************************************************************************//**
 * Construct a new compilier with the token stream initially bound to std::cin.
 * @param	pName	The prefix string used by error and verbose/diagnostic messages.
 ************************************************************************************************/
PasComp::PasComp(const string& pName) : Compilier (pName) {
	// insert builtin types into the symbol table
	symtbl.insert( { "Integer",	SymValue(0, TDesc::intDesc)		} );
	symtbl.insert( { "Real",	SymValue(0, TDesc::realDesc)	} );

	// insert builtin constants into the symbol table
	symtbl.insert( { "IntMax", SymValue::makeConst(0, numeric_limits<int>::max(), TDesc::intDesc) } );
}

