/********************************************************************************************//**
 * @file comp.cc
 *
 * Pascal-Lite Compiler implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include "comp.h"
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
 * class PL0CComp
 ************************************************************************************************/

// private:

/********************************************************************************************//**
 * @param	type	Type descriptor to investagate
 * @return  true if type can be treated as an Integer
 ************************************************************************************************/
bool Comp::isAnInteger(TDescPtr type) {
	return type->kind() == TDesc::Integer;
}

/********************************************************************************************//**
 * @param	type	Type descriptor to investagate
 * @return  true if type can be treated as an Real
 ************************************************************************************************/
bool Comp::isAReal(TDescPtr type) {
	return	type->kind() == TDesc::Real;
}


/********************************************************************************************//**
 * Convert stack operand to a real as necessary. 
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 ************************************************************************************************/
void Comp::promote (TDescPtr lhs, TDescPtr rhs) {
	if (lhs->kind() == rhs->kind())
		;									// nothing to do

	else if ((isAnInteger(lhs) && isAnInteger(rhs)) || (isAReal(lhs) && isAReal(rhs)))
		;									// nothing to do, again

	else if (isAnInteger(lhs) && isAReal(rhs))
		emit(OpCode::ITOR2);				// promote lhs (TOS-1) to a real

	else if (isAReal(lhs) && isAnInteger(rhs))
		emit(OpCode::ITOR);					// promote rhs to a real
	
	else
		error("incompatable binary types");
}

/********************************************************************************************//**
 * Convert rhs of assign to real if necessary, or emit error would need to be
 * converted to an integer. 
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 ************************************************************************************************/
void Comp::assignPromote (TDescPtr lhs, TDescPtr rhs) {
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
TDescPtr Comp::identFactor(int level, const string& id) {
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
TDescPtr Comp::factor(int level) {
	auto type = TDesc::intDesc;			// Factor data type

	if (accept(Token::Identifier, false)) {
		// Copy, and then consume the identifer...
    	const string id = ts.current().string_value;
    	next();

		type = identFactor(level, id);

	} else if (accept(Token::Round))  {	// round(expr) to an integer
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() == TDesc::Real) {
			emit(OpCode::RTOI);
			type = TDesc::intDesc;
		}
		
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

	} else {
		ostringstream oss;
		oss << "factor: syntax error; expected ident | num | { expr }, got: " << current();
		error(oss.str());
		next();
	}

	return type;
}

/********************************************************************************************//**
 * fact { ( * | / | mod | and ) fact } ;
 *
 * @param level		The current block level 
 *
 * @return	Data type  
 ************************************************************************************************/
TDescPtr Comp::term(int level) {
	const auto lhs = factor(level);

	for (;;) {
		if (accept(Token::Multiply)) {
			promote(lhs, factor(level));
			emit(OpCode::Mul);
			
		} else if (accept(Token::Divide)) {
			promote(lhs, factor(level));
			emit(OpCode::Div);	
			
		} else if (accept(Token::Mod)) {
			promote(lhs, factor(level));
			emit(OpCode::Rem);

		} else if (accept(Token::And)) {
			promote(lhs, factor(level));
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
TDescPtr Comp::unary(int level) {
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
TDescPtr Comp::simpleExpr(int level) {
	auto lhs = unary(level);

	for (;;) {
		if (accept(Token::Add)) {
			promote(lhs, unary(level));
			emit(OpCode::Add);

		} else if (accept(Token::Subtract)) {
			promote(lhs, unary(level));
			emit(OpCode::Sub);

		} else if (accept(Token::Or)) {
			promote(lhs, unary(level));
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
TDescPtr Comp::expression(int level) {
	auto lhs = simpleExpr(level);

	for (;;) {
		if (accept(Token::LTE)) {
			promote(lhs, simpleExpr(level));
			emit(OpCode::LTE);

		} else if (accept(Token::LT)) {
			promote(lhs, simpleExpr(level));
			emit(OpCode::LT);

		} else if (accept(Token::GT)) {
			promote(lhs, simpleExpr(level));
			emit(OpCode::GT);
			
		} else if (accept(Token::GTE)) {
			promote(lhs, simpleExpr(level));
			emit(OpCode::GTE);
			
		} else if (accept(Token::EQU)) {
			promote(lhs, simpleExpr(level));
			emit(OpCode::EQU);

		} else if (accept(Token::NEQ)) {
			promote(lhs, simpleExpr(level));
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
TDescPtrVec	Comp::expressionList(int level) {
	TDescPtrVec	v;

	do {
		v.push_back(expression(level));
	} while(accept(Token::Comma));

	return v;
}

/********************************************************************************************//**
 * [ + | - ] number | (const) identifer
 *
 * @return A constant value
 ************************************************************************************************/
Datum Comp::constExpr() {
	Datum value(0);
	int sign = 1;

	if (accept(Token::Add))
		;											// ignore unary + 
	else if (accept(Token::Subtract))
		sign = -1;

	if (accept(Token::IntegerNum, false)) {
		value = sign * ts.current().integer_value;
		next();										// Consume the number

	} else if (accept(Token::RealNum, false)) {
		value = sign * ts.current().real_value;
		next();										// Consume the number

	} else if (accept(Token::Identifier, false)) {
		// Copy, and then, consume the identifier..
		const string id = ts.current().string_value;
		next();

		auto it = lookup(id);
		if (it != symtbl.end()) {
			if (it->second.kind() == SymValue::Constant)
				value = sign * it->second.value();
			else
				error("Identifier is not a constant, variable or function", it->first);
		}

	} else {
		error("expected an interger or real literal, got neither", ts.current().string_value);
		next();										// Consume the errent token
	}

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
void Comp::callStmt(const string& name, const SymValue& val, int level) {
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
 void Comp::whileStmt(int level) {
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
 void Comp::ifStmt(int level) {
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
 void Comp::repeatStmt(int level) {
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
void Comp::statementList(int level) {
	do {
		statement(level);
	} while (accept(Token::SemiColon));
}

/********************************************************************************************//**
 * identifier [ '[' expression-list ']' ]
 *
 * Emits a variable reference, including array indexes.
 *
 * @param	id		The variable or array identifier
 * @param	level	The current block level.
 ************************************************************************************************/
TDescPtr Comp::variable(int level, SymbolTable::iterator it) {
	auto type = it->second.type();

	emitVarRef(level, it->second);
	if (accept(Token::OpenBrkt)) {			// variable is an array, index into it
		auto atype = it->second.type();		// kind s/b TDesc::Array
		type = atype->base();				// Return the array base type...

		if (atype->kind() != TDesc::Array)
			error("attempt to index into non-array", it->first);

		auto tvec = expressionList(level);
		if (tvec.empty())
			error("expected expression-list");

		else if (tvec.size() > 1)
			error("multidimensional arrays are not supported!");

		else if (atype->rtype()->kind() != tvec[0]->kind()) {
			ostringstream oss;				// index isn't the right type
			oss
				<< "incompatable array index type, expected "
				<< atype->rtype()->kind() << " got " << tvec[0]->kind();
			error(oss.str());

		} else if (type->size() != 1) {		// scale the index
			emit(OpCode::Push, 0, type->size());
			emit(OpCode::Mul);
		}

		if (atype->range().minimum() != 0) {	// offset non-zero based array index
			emit(OpCode::Push, 0, atype->range().minimum());
			emit(OpCode::Sub);
		}
		emit(OpCode::Add);					// index into the array
		expect(Token::CloseBrkt);
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
void Comp::identStatement(int level, const string& id) {
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
void Comp::statement(int level) {
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
void Comp::constDeclList(int level) {
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
void Comp::typeDecl(int level) {
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
void Comp::typeDeclList(int level) {
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
void Comp::constDecl(int level) {
	auto ident = nameDecl(level);				// Copy the identifier
	expect(Token::EQU);							// Consume the "="
	auto value = constExpr();					// Get the constant value

	TDescPtr type = value.kind() == Datum::Kind::Integer ? TDesc::intDesc : TDesc::realDesc;

	symtbl.insert(	{ ident, SymValue(level, value, type) }	);
	if (verbose)
		cout << progName << ": constDecl " << ident << ": " << level << ", " << value << "\n";
}

/********************************************************************************************//**
 * const" var-decl-lst ;
 *
 * @param	level	The current block level.
 * @return  Number of variables allocated before or after the activation frame.
 ************************************************************************************************/
int Comp::varDeclBlock(int level) {
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
void Comp::varDeclList(int level, bool params, NameKindVec& idents) {
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

		symtbl.insert( { id.name, SymValue(level, dx++, id.type)	} );
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
void Comp::varDecl(int level, NameKindVec& idents) {

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
vector<string> Comp::identifierList(int level) {
	vector<string> ids;

	do {
		ids.push_back(nameDecl(level));
	} while (accept(Token::Comma));

	return ids;
}

/********************************************************************************************//**
 * type = simple-type | 'real' | 'array' '[' simple-type-list ']' 'of' type ;
 *
 * Previously defined types, as well as  built-in types such as "integer" and "real" will have
 * Token::Type, thus we only need to look for new type declaractions, e.g., "array...".
 *
 * @param	level	The current block level. 
 * @return the type description
 ************************************************************************************************/
TDescPtr Comp::type(int level) {
	auto tdesc = TDesc::intDesc;

	// Make a copy in case the next token is an identifier...
	const string id = ts.current().string_value;

	if (accept(Token::Identifier)) {			// previously defined type-name
		auto it = lookup(id);
		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected type, got ", id);
		else
			tdesc = it->second.type();

	} else if (accept(Token::RealType))			// Real
		return TDesc::realDesc;

	else if (accept(Token::Array)) {			// Array
		expect(Token::OpenBrkt);				// "["
		TDescPtrVec typeVec = simpleTypeList(level);
		assert(!typeVec.empty());
		SubRange r;
		if (typeVec.size() > 1) {
			error("multiple dimension arrays aren't supported!");
			r = SubRange(0, 1);
		} else
			r = typeVec[0]->range();

		expect(Token::CloseBrkt);				// "]"
		expect(Token::Of);

		tdesc = TDesc::newTDesc(TDesc::Array, r.span(), r, typeVec[0], type(level), FieldVec());

	} else										// simple-type
		tdesc = simpleType(level);

	return tdesc;
}

/********************************************************************************************//**
 * simple-type = 'integer' | '(' ident-list ')' | const-expr '..' const-expr
 *
 * @note All simple-types are ordinal types.
 * 
 * @param	level	The current block level. 
 * @return the type description
 ************************************************************************************************/
TDescPtr Comp::simpleType(int level) {
	auto type = TDesc::intDesc;

	// Copy if next token is an identifier...
	const string id = ts.current().string_value;
	if (accept(Token::Identifier)) {			// Previously defined type, must be ordinal!
		auto it = lookup(id);
		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected type, got ", it->first);
		else if (!it->second.type()->isOrdinal())
			error("expected ordinal type, got ", it->first);
		else
			type = it->second.type();

	} else if (accept(Token::IntType))			// Integer
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
			symtbl.insert(	{ id, SymValue(level, value, t) }	);
			if (verbose)
				cout << progName << ": enumeration '" << id << "' = " << value << ", " << level << "\n";
			++value;
		}

		t->fields(enums);
		type = t;

	} else {									// Sub-Range
		auto minValue = constExpr();
		expect(Token::Ellipsis);
		auto maxValue = constExpr();

		ostringstream oss;
		if (minValue > maxValue) {
			oss << "Minimum sub-range value (" << minValue << ")"
				<< " is greater than the maximum value (" << maxValue << ")";
			error(oss.str());
			swap(minValue, maxValue);

		} else if (minValue.kind() != Datum::Kind::Integer || maxValue.kind() != Datum::Kind::Integer) {
			oss << "Both sub-range values must be ordinal types; " << minValue << ", " << maxValue;
			error(oss.str());
			minValue = 0; maxValue = 1;
		}

		SubRange r(minValue.integer(), maxValue.integer());

		type = TDesc::newTDesc(TDesc::Integer, 1, r);
	}

	return type;
}


/********************************************************************************************//**
 * simple-type-lst: simple-type { ',' simple-type } ;
 *
 * @param	level	The current block level. 
 ************************************************************************************************/
TDescPtrVec Comp::simpleTypeList(int level) {
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
SymValue& Comp::subPrefixDecl(int level, SymValue::Kind kind) {
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
void Comp::procDecl(int level) {
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
void Comp::funcDecl(int level) {
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
 *     type ; type =    "integer" | "real" ;
 *
 * @param level The current block level.
 ************************************************************************************************/
void Comp::subDeclList(int level) {
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
unsigned Comp::blockDecl(SymValue& val, int level) {
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
void Comp::run() {
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
Comp::Comp(const string& pName) : CompBase (pName) {
	// insert builtin types into the symbol table
	symtbl.insert( { "integer",	SymValue(0, TDesc::intDesc)		} );
	symtbl.insert( { "real",	SymValue(0, TDesc::realDesc)	} );
}

