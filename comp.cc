/********************************************************************************************//**
 * @file comp.cc
 *
 * P Language Compiler implementation
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
using namespace rel_ops;

/************************************************************************************************
 * class PComp
 ************************************************************************************************/

// private:

/********************************************************************************************//**
 * @param	type	Type descriptor to investagate
 * @return  true if type can be treated as an Integer
 ************************************************************************************************/
bool PComp::isAnInteger(TDescPtr type) {
	return type->tclass() == TypeDesc::Integer;
}

/********************************************************************************************//**
 * @param	type	Type descriptor to investagate
 * @return  true if type can be treated as an Real
 ************************************************************************************************/
bool PComp::isAReal(TDescPtr type) {
	return	type->tclass() == TypeDesc::Real;
}

/********************************************************************************************//**
 * @param	where	The jump to address, if known
 * @return  address	of the jump to address instruction, for patching when know
 ************************************************************************************************/
size_t PComp::emitJump(size_t where) {
	const size_t addr = emit(OpCode::PUSH, 0, where);
	emit(OpCode::JUMP);
	return addr;
}

/********************************************************************************************//**
 * @param	where	The jump to address, if known
 * @return  address	of the jump to address instruction, for patching when know
 ************************************************************************************************/
size_t PComp::emitJumpI(size_t where) {
	return emit(OpCode::JUMPI, 0, where);
}

/********************************************************************************************//**
 * @param	where	The jump to address, if known
 * @return  address	of the jump to address instruction, for patching when know
 ************************************************************************************************/
size_t PComp::emitJNEQ(size_t where) {
	const size_t addr = emit(OpCode::PUSH, 0, where);
	emit(OpCode::JNEQ);
	return addr;
}

/********************************************************************************************//**
 * @param	where	The jump to address, if known
 * @return  address	of the jump to address instruction, for patching when know
 ************************************************************************************************/
size_t PComp::emitJNEQI(size_t where) {
	return emit(OpCode::JNEQI, 0, where);
}

/********************************************************************************************//**
 * @param	level	The new block level
 * @param	where	The subroutine entry point, if known
 * @return  address	of the call address instruction, for patching when known
 ************************************************************************************************/
size_t PComp::emitCall(int8_t level, size_t where) {
	emit(OpCode::PUSH, 0, level);
	const size_t addr = emit(OpCode::PUSH, 0, where);
	emit(OpCode::CALL);

	return addr;
}

/********************************************************************************************//**
 * @param	level	The new block level
 * @param	where	The subroutine entry point, if known
 * @return  address	of the call address instruction, for patching when known
 ************************************************************************************************/
size_t PComp::emitCallI(int8_t level, size_t where) {
	return emit(OpCode::CALLI, level, where);
}

/********************************************************************************************//**
 * Promote binary stack operands as necessary. 
 *
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 *
 * @return	Type type of the promoted type
 ************************************************************************************************/
TDescPtr PComp::promote(TDescPtr lhs, TDescPtr rhs) {
	TDescPtr type = lhs;					// Assume that lhs and rhs have the same types
	if (lhs->tclass() == rhs->tclass())
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
void PComp::assignPromote (TDescPtr lhs, TDescPtr rhs) {
	if (lhs->tclass() == rhs->tclass())
		;				// nothing to do

	else if ((isAnInteger(lhs) && isAnInteger(rhs)) || (isAReal(lhs) && isAReal(rhs)))
		;				// nothing to do, again

	else if (isAnInteger(lhs) && isAReal(rhs)) {
		error("rounding real to fit in an integer");
		emit(OpCode::ROUND);				// promote rhs to a integer	 

	} else if (isAReal(lhs) && isAnInteger(rhs))
		emit(OpCode::ITOR);                 // promote rhs to a real

	else
		error("incompatable assignment types");

	// Emit limit checks, unless range is impossible to exceed
	if (lhs->ordinal() && lhs->range() != TypeDesc::maxRange) {
		emit(OpCode::LLIMIT, 0, lhs->range().min());
		emit(OpCode::ULIMIT, 0, lhs->range().max());
	}
}

/********************************************************************************************//**
 * Push a variable's value, a constant value, or invoke a function, and push the results,
 * of a function.
 *
 * ident | ident [ ( expr-lst ) ]
 *
 * @param	id		The variable or prodecure identifier
 * @param	level	The current block level 
 * @param	var		True if factor will be passed to a var parameter
 *
 * @return	Data type 
 ************************************************************************************************/
TDescPtr PComp::identFactor(int level, const string& id, bool var) {
	auto type = TypeDesc::newIntDesc();
   	auto it = lookup(id);

	if (it != symtbl.end()) {
		switch (it->second.kind()) {
		case SymValue::Type:
			if (var) error("can't pass attribute value by reference");
			expect(Token::Tick);
			type = attribute(it, it->second.type());
			break;

		case SymValue::Constant:
			type = it->second.type();
			emit(OpCode::PUSH, 0, it->second.value());
			if (var) error("attempt to pass constant by reference!");
			assert(!type->ref());
			break;

		case SymValue::Variable:
			type = variable(level, it);
			assert(type != nullptr);
			assert(type->base() != nullptr);
			type = type->base();
			assert(type != nullptr);
			if (!var)
				emit(OpCode::EVAL, 0, type->size());
			if (type->ref())
				emit(OpCode::EVAL, 0, type->size());
			break;

		case SymValue::Function:
			type = it->second.type();		// Use the function return type
			callStatement(level, it);
			if (var) error("attempt to pass function return by reference!");
			if (type->ref())
				error("passing functions by reference isn't supported!");
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
TDescPtr PComp::builtInFunc(int level)
{
	auto type = TypeDesc::newIntDesc();	// Factor data type
	ostringstream oss;

	if (accept(Token::Round))  {		// round(expr) to an integer
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() != TypeDesc::Real) {
			oss << "expeced real value, got: " << current();
			error(oss.str());

		} else {
			emit(OpCode::ROUND);
			type = TypeDesc::newIntDesc();
		}

	} else if (accept(Token::Trunc)) {	// truncate(expr) to an integer
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() != TypeDesc::Real) {
			oss << "expeced real value, got: " << current();
			error(oss.str());

		} else {
			emit(OpCode::TRUNC);
			type = TypeDesc::newIntDesc();
		}

	} else if (accept(Token::Abs)) {	// replace TOS with it's absolute value
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() != TypeDesc::Integer && type->tclass() != TypeDesc::Real)
			oss << "expeced integer or real value, got: " << current();

		else
			emit(OpCode::ABS);

	} else if (accept(Token::Atan)) {	// replace TOS with it's arc tangent
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() == TypeDesc::Integer)
			type = TypeDesc::newRealDesc();		// Always produces a real
		else if (type->tclass() != TypeDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::ATAN);

	} else if (accept(Token::Exp)) {	// Replace TOS with e raised to the given power TOS
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() == TypeDesc::Integer)
			type = TypeDesc::newRealDesc();		// Always produces a real
		else if (type->tclass() != TypeDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::EXP);

	} else if (accept(Token::Log)) {	// Replace TOS with log(TOS)
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() == TypeDesc::Integer)
			type = TypeDesc::newRealDesc();		// Always produces a real
		else if (type->tclass() != TypeDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::LOG);

	} else if (accept(Token::Odd)) {	// Replace TOS with is TOS odd?
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() != TypeDesc::Integer)
			oss << "expeced integer value, got: " << current();
		emit(OpCode::ODD);

	} else if (accept(Token::Pred)) {	// Replace TOS with its predicessor
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->ordinal())
			oss << "expeced ordinal value, got: " << current();
		emit(OpCode::PRED, 0, type->range().min());

	} else if (accept(Token::Sin)) {	// Replace TOS with sin(TOS)
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() == TypeDesc::Integer)
			type = TypeDesc::newRealDesc();		// Always produces a real
		else if (type->tclass() != TypeDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::SIN);

	} else if (accept(Token::Sqr)) {	// Replace TOS with TOS * TOS
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() == TypeDesc::Integer)
			type = TypeDesc::newRealDesc();		// Always produces a real
		else if (type->tclass() != TypeDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::SQR);

	} else if (accept(Token::Sqrt)) {	// Replace TOS with sqrt(TOS)
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->tclass() == TypeDesc::Integer)
			type = TypeDesc::newRealDesc();		// Always produces a real
		else if (type->tclass() != TypeDesc::Real)
			oss << "expeced integer, or real value, got: " << current();
		emit(OpCode::SQRT);

	} else if (accept(Token::Succ)) {	// Replace TOS with its successor
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->ordinal())
			oss << "expeced ordinal value, got: " << current();
		emit(OpCode::SUCC, 0, type->range().max());

	} else if (accept(Token::Ord)) {	// replace ordinal with it's ordinal
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (!type->ordinal()) {
			oss << "expected ordinal, got: " << current();
			error(oss.str());
		} else 
			type = TypeDesc::newIntDesc();

	} else {
		oss << "bultInFunc: syntax error; expected ident | num | { expr }, got: " << current();
		error(oss.str());
		next();
	}
		
	return type;
}

/********************************************************************************************//**
 * identifier | number | not factor |  character | string | ident ( expr-list) | ( expression )
 * 
 * @param	level	The current block level.
 * @param	var		True if factor will be passed to a var parameter
 *
 * @return	Data type 
 ************************************************************************************************/
TDescPtr PComp::factor(int level, bool var) {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "factor(" << level << ", " << boolalpha << var << ")\n";

	auto type = TypeDesc::newIntDesc();			// Factor data type
	if (accept(Token::Identifier, false)) {		// Copy, and then consume the identifer...
    	const string id = ts.current().string_value;
    	next();
		type = identFactor(level, id, var);

	} else if (accept(Token::IntegerNum, false)) {
		int value = ts.current().integer_value;
		if (var) error("can't take reference of a literal");
		emit(OpCode::PUSH, 0, value);
		expect(Token::IntegerNum);
		type = TypeDesc::newIntDesc();

	} else if (accept(Token::RealNum, false)) {
		if (var) error("can't take reference of a literal");
		emit(OpCode::PUSH, 0, ts.current().real_value);
		expect(Token::RealNum);
		type = TypeDesc::newRealDesc();

	} else if (accept(Token::OpenParen)) {
		type = expression(level, var);
		expect(Token::CloseParen);

	} else if (accept(Token::Not)) {
		emit(OpCode::NOT);
		type = factor(level, var);

	} else if (accept(Token::Character, false)) {
		const string s = ts.current().string_value;
		next();

		if (s.empty())
			error("expected a character, but got ''");

		else {
			emit(OpCode::PUSH, 0, s[0]);
			type = TypeDesc::newCharDesc();
		}

	} else if (accept(Token::String, false)) {
		const string s = ts.current().string_value;
		next();

		if (s.empty()) {
			error("expected a string, but got got \"\"");

		} else {								// push each character...
			for (char c : s)
				emit(OpCode::PUSH, 0, c);
			type = TypeDesc::newArrayDesc(	s.size(),
											Subrange(0, s.size() - 1),
											TypeDesc::newIntDesc(),
											TypeDesc::newCharDesc());
		}

	} else 
		type = builtInFunc(level);

	return type;
}

/********************************************************************************************//**
 * fact { ( * | / | mod | bit_and | and ) fact } ;
 *
 * @param	level	The current block level 
 * @param	var		True if term will be passed to a var parameter
 *
 * @return	Data type  
 ************************************************************************************************/
TDescPtr PComp::term(int level, bool var) {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "terminal(" << level << ',' << boolalpha << var << ")\n";

	auto lhs = factor(level, var);
	for (;;) {
		if (accept(Token::Multiply)) {
			lhs = promote(lhs, factor(level, var));
			emit(OpCode::MUL);
			
		} else if (accept(Token::Divide)) {
			lhs = promote(lhs, factor(level, var));
			emit(OpCode::DIV);	
			
		} else if (accept(Token::Mod)) {
			lhs = promote(lhs, factor(level, var));
			emit(OpCode::REM);

		} else if (accept(Token::BitAnd)) {
			lhs = promote(lhs, factor(level, var));
			emit(OpCode::BAND);
			
		} else if (accept(Token::And)) {
			lhs = promote(lhs, factor(level, var));
			emit(OpCode::AND);

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * [ ( + | - | bit_not ] term
 *
 * @param	level	The current block level 
 * @param	var		True if unary will be passed to a var parameter
 *
 * @return	Data type.
 ************************************************************************************************/
TDescPtr PComp::unary(int level, bool var) {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "unary(" << level << ',' << boolalpha << var << ")\n";

	auto type = TypeDesc::newIntDesc();		// Default factor data type
	if (accept(Token::Add)) 
		type = term(level, var);			// ignore unary + 

	else if (accept(Token::Subtract)) {
		type = term(level, var);
		emit(OpCode::NEG);

	} else if (accept(Token::BitNot)) {
		type = term(level, var);
		emit(OpCode::BNOT);

	} else									
		type = term(level, var);

	return type;
}

/********************************************************************************************//**
 * term { (+ | - | bit_or | bit_xor | bit_sleft | bit_sright | or ) term } ;
 *
 * @param	level	The current block level. 
 * @param	var		True if simple-expr will be passed to a var parameter
 *
 * @return	Data type.
 ************************************************************************************************/
TDescPtr PComp::simpleExpr(int level, bool var) {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "simple-expr(" << level << ',' << boolalpha << var << ")\n";

	auto lhs = unary(level, var);
	for (;;) {
		if (accept(Token::Add)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::ADD);

		} else if (accept(Token::Subtract)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::SUB);

		} else if (accept(Token::BitOr)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::BOR);

		} else if (accept(Token::BitXor)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::BXOR);

		} else if (accept(Token::ShiftLeft)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::SHIFTL);

		} else if (accept(Token::ShiftRight)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::SHIFTR);

		} else if (accept(Token::Or)) {
			lhs = promote(lhs, unary(level, var));
			emit(OpCode::OR);

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * simpleExpr { ( < | <= | = | >= | > | <> ) simpleExpr } ;
 *
 * @param	level	The current block level 
 * @param	var		True if expression will be passed to a var parameter
 *
 * @return	Data type.
 ************************************************************************************************/
TDescPtr PComp::expression(int level, bool var) {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "expresson(" << level << ',' << boolalpha << var << ")\n";

	auto lhs = simpleExpr(level, var);
	for (;;) {
		if (accept(Token::LTE)) {
			lhs = promote(lhs, simpleExpr(level, var));
			emit(OpCode::LTE);

		} else if (accept(Token::LT)) {
			lhs = promote(lhs, simpleExpr(level, var));
			emit(OpCode::LT);

		} else if (accept(Token::GT)) {
			lhs = promote(lhs, simpleExpr(level, var));
			emit(OpCode::GT);
			
		} else if (accept(Token::GTE)) {
			lhs = promote(lhs, simpleExpr(level, var));
			emit(OpCode::GTE);
			
		} else if (accept(Token::EQU)) {
			lhs = promote(lhs, simpleExpr(level, var));
			emit(OpCode::EQU);

		} else if (accept(Token::NEQ)) {
			lhs = promote(lhs, simpleExpr(level, var));
			emit(OpCode::NEQ);

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
TDescPtrVec	PComp::expressionList(int level) {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "expression-list(" << level << ")\n";

	TDescPtrVec	v;
	do {
		v.push_back(expression(level));
	} while(accept(Token::Comma));

	return v;
}

/********************************************************************************************//**
 * Promote binary constant-expression  operands as necessary. 
 *
 * Converts lhs or rhs to a Real as necessary.
 *
 * @param	lhs	The left-hand-side operand
 * @param	rhs	The right-hand-side  operand
 ************************************************************************************************/
void PComp::constPromote(Datum& lhs, Datum& rhs) {
	if (lhs.kind() == lhs.kind())
		;									// nothing to do

	else if ((lhs.kind() == Datum::Integer	&& rhs.kind() == Datum::Integer) ||
			 (lhs.kind() == Datum::Real		&& rhs.kind() == Datum::Real))
		;									// nothing to do, again

	else if (lhs.kind() == Datum::Integer && rhs.kind() == Datum::Real)
		lhs = lhs.integer() * 1.0;			// convert lhs to a real

	else if (lhs.kind() == Datum::Real && rhs.kind() == Datum::Integer)
		rhs = rhs.integer() * 1.0;			// convert rhs to a real

	else
		error("incompatable binary types");
}

/********************************************************************************************//**
 * identifier | number | ( const-expression )
 *
 * @return A boolean, constant value pair. Second is valid only if first is true.
 ************************************************************************************************/
PComp::ConstExprValue PComp::constFactor() {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "constant-factor ()\n";

	ConstExprValue value = make_pair(true, Datum(0));
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

	} else if (accept(Token::Identifier, false)) {	// Copy, and then, consume the identifier..
		auto it = lookup(ts.current().string_value);
		next(); 									// consume the identifier

		switch (it->second.kind()) {
		case SymValue::Type: {
			expect(Token::Tick);
			const string attrib = ts.current().string_value;
			if (expect(Token::Identifier)) {
				if (attrib == "min") {
					if (it->second.type()->tclass() == TypeDesc::Pointer)
						error("min attribute not defined for pointers", it->first);
					else if (it->second.type()->tclass() == TypeDesc::Record)
						error("min attribute not defined for records", it->first);
					else if (it->second.type()->tclass() == TypeDesc::Real)
						value.second = numeric_limits<double>::min();
					else 
						value.second = it->second.type()->range().min();

				} else if (attrib == "max") {
					if (it->second.type()->tclass() == TypeDesc::Pointer)
						error("max attribute not defined for pointers", it->first);
						else if (it->second.type()->tclass() == TypeDesc::Record)
							error("max attribute not defined for records", it->first);
						else if (it->second.type()->tclass() == TypeDesc::Real)
							value.second = numeric_limits<double>::max();
						else
							value.second = it->second.type()->range().max();
					}
				}
			}
			break;

		case SymValue::Constant:
			value.second = Datum(sign) * it->second.value();
			break;

		default:
			error("Identifier is not a constant or type attribute ", it->first);
		}

	} else if (accept(Token::OpenParen)) {
		value = constExpr();
		expect(Token::CloseParen);

	} else
		value.first = false;
		
	return value;
}

/********************************************************************************************//**
 * const-fact { ( * | / | mod | and ) const-fact } ;
 *
 * @return A boolean, constant value pair. Second is valid only if first is true.
 ************************************************************************************************/
PComp::ConstExprValue PComp::constTerm() {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "constant-terminal()\n";

	ConstExprValue lhs = constFactor();
	ConstExprValue rhs = make_pair(false, Datum(0));
	for (;;) {
		if (accept(Token::Multiply)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second *= rhs.second;
			
		} else if (accept(Token::Divide)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second /= rhs.second;
			
		} else if (accept(Token::Mod)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second *= rhs.second;

		} else if (accept(Token::BitAnd)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second *= rhs.second;
			
		} else if (accept(Token::And)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second *= rhs.second;

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * [ ( + | - | bit_not ] const-term
 *
 * @return A boolean, constant value pair. Second is valid only if first is true.
 ************************************************************************************************/
PComp::ConstExprValue PComp::constUnary() {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "constant-unary()\n";

	ConstExprValue value = make_pair(false, Datum(0));
	if (accept(Token::Add)) 
		value = constTerm();				// ignore unary + 

	else if (accept(Token::Subtract)) {
		value = constTerm();
		value.second = -value.second;

	} else if (accept(Token::BitNot)) {
		value = constTerm();
		value.second = ~value.second;

	} else									
		value = constTerm();

	return value;
}

/********************************************************************************************//**
 * const-unary { + | - | | | ^ | bit_sleft | bit_sright | or ;
 *
 * @return A boolean, constant value pair. Second is valid only if first is true.
 ************************************************************************************************/
PComp::ConstExprValue PComp::constSimpleExpr() {
	LogLevel	lvl;
	if (verbose)
		cout << prefix(progName) << "constant-simple-expression()\n";

	ConstExprValue lhs = constUnary();
	ConstExprValue rhs = make_pair(false, Datum(0));
	for (;;) {
		if (accept(Token::Add)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second += rhs.second;

		} else if (accept(Token::Subtract)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second - rhs.second;

		} else if (accept(Token::BitOr)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second |= rhs.second;

		} else if (accept(Token::BitXor)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second ^= rhs.second;

		} else if (accept(Token::ShiftLeft)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second = lhs.second << rhs.second;

		} else if (accept(Token::ShiftRight)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second = lhs.second >> rhs.second;

		} else if (accept(Token::Or)) {
			rhs = constSimpleExpr();
			constPromote(lhs.second, rhs.second);
			lhs.second = lhs.second || rhs.second;

		} else
			break;
	}

	return lhs;
}

/********************************************************************************************//**
 * const-simple-expr { <= | < | > | >= | = | <> const-simple-expr
 *
 * @return A boolean, constant value pair. Second is valid only if first is true.
 ************************************************************************************************/
PComp::ConstExprValue PComp::constExpr() {
	LogLevel	lvl;

	if (verbose)
		cout << prefix(progName) << "constant-expression()\n";

	ConstExprValue lhs = constSimpleExpr();
	ConstExprValue rhs = make_pair(false, Datum(0));
	if (lhs.first) {
		for (;;) {
			if (accept(Token::LTE)) {
				rhs = constSimpleExpr();
				constPromote(lhs.second, rhs.second);
				lhs.second = lhs.second <= rhs.second;

			} else if (accept(Token::LT)) {
				rhs = constSimpleExpr();
				constPromote(lhs.second, rhs.second);
				lhs.second = lhs.second <  rhs.second;

			} else if (accept(Token::GT)) {
				rhs = constSimpleExpr();
				constPromote(lhs.second, rhs.second);
				lhs.second = lhs.second >  rhs.second;

			} else if (accept(Token::GTE)) {
				rhs = constSimpleExpr();
				constPromote(lhs.second, rhs.second);
				lhs.second = lhs.second >= rhs.second;
			
			} else if (accept(Token::EQU)) {
				rhs = constSimpleExpr();
				constPromote(lhs.second, rhs.second);
				lhs.second = lhs.second == rhs.second;

			} else if (accept(Token::NEQ)) {
				rhs = constSimpleExpr();
				constPromote(lhs.second, rhs.second);
				lhs.second = lhs.second != rhs.second;

			} else
				break;
		}
	}

	return lhs;
}

/********************************************************************************************//**
 * Call a function or procedure...
 *
 * ident [ '(' expr-list ')' ]...
 *
 * @param	level	The current block level 
 * @param	it		The sub-routines symbol table entry
 ************************************************************************************************/
void PComp::callStatement(int level, SymbolTableIter it) {
	if (expect(Token::OpenParen)) {
		unsigned nParams = 0;					// Count actual parameters

		const auto& params = it->second.params(); // Formal parameter kinds
		if (!accept(Token::CloseParen, false))
			do {								// collect actual parameters
				if (params.size() > nParams) {
					const auto kind = expression(level, params[nParams]->ref());
					assignPromote(params[nParams], kind);
				} else
					expression(level); 			// consume the expression...

				++nParams;

			} while (accept (Token::Comma));

		expect(Token::CloseParen);

		if (nParams != params.size()) {			// Is the caller passing right # of params?
			ostringstream oss;
			oss << "passing " << nParams << " parameters, where " << params.size() << " expected";
			error(oss.str());
		}
	}

	if (SymValue::Procedure != it->second.kind() && SymValue::Function != it->second.kind())
		error("Identifier is not a function or procedure", it->first);

	emitCallI(level - it->second.level(), it->second.value().natural());
}

/********************************************************************************************//**
 * statement { ; statement }
 *
 * @param	level		The current block level.
 * @param	it			The enclosing subroutine context
 ************************************************************************************************/
void PComp::statementList(int level, SymbolTableEntry& context) {
	static const Token::KindSet stops {
		Token::End,
		Token::Endfunc,
		Token::Endproc,
		Token::Endprog
	};

	do {
		if (oneOf(stops))
			break;
		statement(level, context);
	} while (accept(Token::SemiColon));
}

/********************************************************************************************//**
 * Array index expression-lst.
 *
 * Process a possibly multi-dimensional,  array index. The opening bracket has already been
 * consumed, and the caller will consume consume the closing bracket.
 *
 * @param	level	The current block level.
 * @param	it		The arrays's entry into the symbol table
 * @param	type	The array's type
 *
 * @return	The arrays base type
 ************************************************************************************************/
TDescPtr PComp::varArray(int level, SymbolTableIter it, TDescPtr type) {
	TDescPtr atype = type;					// The arrays type, e.g, ArrayDesc
	type = atype->base();					// We'll return the arrays base type...

	if (atype->tclass() != TypeDesc::Array)
		error("attempt to index into non-array", it->first);

	auto indexes = expressionList(level);	// expr {, expr }...
	if (indexes.empty())
		error("expected expression-list");

	unsigned nindexes = indexes.size();
	for (auto index : indexes) {			// process each index, in turn
		emit(OpCode::LLIMIT, 0, atype->range().min());
		emit(OpCode::ULIMIT, 0, atype->range().max());

		if (atype->itype()->tclass() != index->tclass()) {
			ostringstream oss;
			oss	<< "incompatable array index type, expected "
				<< atype->itype()->tclass() << " got " << index->tclass();
			error(oss.str());
					
		} else if (type->size() != 1) {		// scale the index, if necessary
			emit(OpCode::PUSH, 0, type->size());
			emit(OpCode::MUL);		// scale the index
		}

		// offset index for non-zero based arrays
		if (atype->range().min() != 0) {
			emit(OpCode::PUSH, 0, atype->range().min());
			emit(OpCode::SUB);
		}

		emit(OpCode::ADD);					// index into the array
				
		if (--nindexes) {					// link to next (base) if there's another index
			atype = type;					// The arrays type
			type = atype->base();			// We'll return the arrays base type...
		}
	}

	return type;
}

/********************************************************************************************//**
 * Emits a selector reference; the '.' has already been consumed
 *
 * @param	it		Variable's entry into the symbol table
 * @param 	type	The owning record type
 * @return	The identifier type
 ************************************************************************************************/
TDescPtr PComp::varSelector(SymbolTableIter it, TDescPtr type) {
	if (type->tclass() != TypeDesc::Record)
		error("attempted selector reference into non-record", it->first);

	// Copy, and then consume, the selector identifier...
	const string selector = ts.current().string_value;
	if (expect(Token::Identifier)) {
		size_t offset = 0;					// Calc the offset into the record...
		for (const auto& fld : type->fields()) {
			if (fld.name() == selector) {
				type = fld.type();			// Return the record field's type
				break;
			}
			offset += fld.type()->size();
		}

		if (offset > 0) {				// Don't bother if it's the 1st field...
			emit(OpCode::PUSH, 0, offset);
			emit(OpCode::ADD);
		}
	}

	return type;
}

/********************************************************************************************//**
 * Handle a attribute reference, emitting the result. The '`' has already been consumed
 *
 * @param	it		Variable's entry into the symbol table
 * @param 	type	The owning record type
 * @return	The identifier type
 ************************************************************************************************/
TDescPtr PComp::attribute(SymbolTableIter it, TDescPtr type) {
	// Copy, and then consume, the attribute identifier...
	const string attrib = ts.current().string_value;
	if (expect(Token::Identifier)) {
		if (attrib == "min") {
			if (type->tclass() == TypeDesc::Pointer) {
				error("min attribute not defined for pointers", it->first);
				emit(OpCode::PUSH, 0, 0);
			} else if (type->tclass() == TypeDesc::Record) {
				error("min attribute not defined for records", it->first);
				emit(OpCode::PUSH, 0, 0);
			} else if (type->tclass() == TypeDesc::Real)
				emit(OpCode::PUSH, 0, numeric_limits<double>::min());
			else if (type->tclass() == TypeDesc::Array) {
				emit(OpCode::PUSH, 0, type->range().min());
				type = type->itype();		// Return arrays index type
			} else
				emit(OpCode::PUSH, 0, type->range().min());

		} else if (attrib == "max") {
			if (type->tclass() == TypeDesc::Pointer) {
				error("max attribute not defined for pointers", it->first);
				emit(OpCode::PUSH, 0, 0);
			} else if (type->tclass() == TypeDesc::Record) {
				error("max attribute not defined for records", it->first);
				emit(OpCode::PUSH, 0, 0);
			} else if (type->tclass() == TypeDesc::Real)
				emit(OpCode::PUSH, 0, numeric_limits<double>::max());
			else if (type->tclass() == TypeDesc::Array) {
				emit(OpCode::PUSH, 0, type->range().max());
				type = type->itype();		// Return arrays index type
			} else
				emit(OpCode::PUSH, 0, type->range().max());

		} else 
			error("unknown attribute", attrib);

	} else
		emit(OpCode::PUSH, 0, 0);

	return type;
}

/********************************************************************************************//**
 * variable		  = identifier [ composite-desc { composite-desc } ] ;
 * composite-desc = '[' expression-lst ']' | '.' identifier ;
 *
 * Emits a variable reference, i.e., a pointer type, optionally with array indexes.
 *
 * @param	id		The variable or array identifier
 * @param	level	The current block level.
 * @return	The identifier type
 ************************************************************************************************/
TDescPtr PComp::variable(int level, SymbolTableIter it) {
	TDescPtr type = emitVarRef(level, it->second);
	if (it->second.type()->ref())			// dereference if necessary...
		emit(OpCode::EVAL, 0, type->size());

	// process composite-desc's, if any...
	for (;;) {
		if (accept(Token::OpenBrkt)) {		// variable is an array, index into it
			type = TypeDesc::newPointerDesc(varArray(level, it, type->base()));
			expect(Token::CloseBrkt);

		} else if (accept(Token::Period))	// handle record selector...
			type = TypeDesc::newPointerDesc(varSelector(it, type->base()));

		else if (accept(Token::Caret)) {	// 'Dereference' pointer (use pointed to type)
			emit(OpCode::EVAL, 0, type->base()->size());	
			assert(type->base() != nullptr);
			type = type->base();

		} else
			break;							// no more composite-desc(s)
	}

	return type;
}

/********************************************************************************************//**
 * @param	level	The current block level.
 * @param	it		The variable, or function, identifier
 * @param	dup		Duplicate reference to the variable if true
 *
 * @return	Reference to the variable reference
 ************************************************************************************************/
TDescPtr PComp::lvalueRef(int level, SymbolTableIter it, bool dup) {
	TDescPtr type = it->second.type();		// Start with the lvalue type

	// Emit a variable or a function reference...
	if (it->second.kind() == SymValue::Function) {
		emit(OpCode::PUSHVAR, 0, FrameRetVal);
		type = TypeDesc::newPointerDesc(type);

	} else if (it->second.kind() == SymValue::Variable)
		type = variable(level, it);

	else
		error("expected a variable or a function, got:", it->first);

	if (dup)
		emit(OpCode::DUP);

	return type;
}

/********************************************************************************************//**
 * variable := expression 
 *
 * @param	level	The current block level.
 * @param	it		The variable, or function, identifier
 * @param	dup		Duplicate reference to the variable if true
 *
 * @return	Reference to the variable reference
 ************************************************************************************************/
void PComp::assignStatement(int level, SymbolTableIter it, bool dup) {
	TDescPtr type = lvalueRef(level, it, dup);
	expect(Token::Assign);

	// Emit the r-value and assignment...

	auto rtype = expression(level);
	assignPromote(type->base(), rtype);
	emit(OpCode::ASSIGN, 0, type->base()->size());
	if (it->second.kind() == SymValue::Function)
		it->second.returned(true);
}

/********************************************************************************************//**
 * variable := expression | identifier [ '(' expression-list ')' ]
 *
 * Emits code for an assignment statement, or a procedure call...
 *
 * @param	level	The current block level.
 *
 * @return	true if	assignment or procedure call
 ************************************************************************************************/
bool PComp::identStatement(int level) {
	if (accept(Token::Identifier, false)) {
		auto lhs = lookup(ts.current().string_value);
		next();

		if (lhs != symtbl.end()) {			// unidentified identifier?
			switch(lhs->second.kind()) {
			case SymValue::Procedure:		// emit a procedure call
				callStatement(level, lhs);
				break;

			case SymValue::Variable:		// emit a reference to a variable, or array, value
				assignStatement(level, lhs);
				break;

			case SymValue::Constant:
				error("Can't assign to a constant", lhs->first);
				break;

			default:
				error("expected variable or procedure call, got", lhs->first);
			}
		}

		return true;
	}

	return false;
}

/********************************************************************************************//**
 * if expr then statement-lst [ else statement-lst ]
 *
 * @param	level 	The current block level
 ************************************************************************************************/
bool PComp::ifStatement(int level, SymbolTableEntry& context) {
	if (accept(Token::If)) {				// if expr then stmt { then stmt... }
		vector<size_t> jmp_end;					// Jump to the end locations..

		expression(level);						// Condition expression
		size_t jmp_pc = emitJNEQI();			// Jump if condition is false
		expect(Token::Then);					// Consume "then"
		statementList(level, context);			// Statements...

		while (accept(Token::Elif)) {			// 0 or more elif...
			jmp_end.push_back(emitJumpI());		// Jump to the end...

			if (verbose)						// jump here if above condition was false
				cout << prefix(progName) << "patching address at " << jmp_pc << " to " << code->size() << '\n';
			(*code)[jmp_pc].value = code->size();

			expression(level);					// Condition expression
			jmp_pc = emitJNEQI();				// Jump if condition is false
			expect(Token::Then);				// Consume "then"
			statementList(level, context);		// Statements...
		}

		if (accept(Token::Else)) {
			jmp_end.push_back(emitJumpI());		// Jump to the end...

			if (verbose)						// Jump here if above condition was false
				cout << prefix(progName) << "patching address at " << jmp_pc << " to " << code->size() << '\n';
			(*code)[jmp_pc].value = code->size();
			jmp_pc = 0;

			statementList(level, context);		// Statements...
		}

		if (jmp_pc) {
			if (verbose)							// Final jump over to here...
				cout << prefix(progName) << "patching address at " << jmp_pc << " to " << code->size() << '\n';
			(*code)[jmp_pc].value = code->size();
		}

		for (size_t pc : jmp_end) {				// Patch jumps to here
			if (verbose)
				cout << prefix(progName) << "patching address at " << pc << " to " << code->size() << '\n';
			(*code)[pc].value = code->size();
		}

		expect(Token::Endif);

		return true;
	}

	return false;
}

/********************************************************************************************//**
 * while expr do statement...
 *
 * @param	level	The current block level.
 * @param	context	The enclosing subroutine context
 ************************************************************************************************/
bool PComp::whileStatement(int level, SymbolTableEntry& context) {
	if (accept(Token::While)) {
		const auto cond_pc = code->size();	// Start of while expr
		expression(level);

		// jump if expr is false...
		const auto jmp_pc = emitJNEQI();
		expect(Token::Loop);				// consume "loop"
		statementList(level, context);
		expect(Token::Endloop);

		emitJumpI(cond_pc);					// Jump back to expr test...

		if (verbose)
			cout << prefix(progName) << "patching address at " << jmp_pc << " to " << code->size() << '\n';
		(*code)[jmp_pc].value = code->size(); 

		return true;
	}

	return false;
}

/********************************************************************************************//**
 * repeat statement until expr
 *
 * @param	level 	The current block level
 * @param	context	The enclosing subroutine context
 ************************************************************************************************/
bool PComp::repeatStatement(int level, SymbolTableEntry& context) {
	if (accept(Token::Repeat)) {
		const size_t loop_pc = code->size();			// jump here until expr fails
		statementList(level, context);
		expect(Token::Until);
		expression(level);
		emitJNEQI(loop_pc);
		expect(Token::Endloop);

		return true;
	}

	return false;
}

/********************************************************************************************//**
 * for identifier := expresson ( to | downto ) condition do statement
 *
 * @param	level	The current block level.
 * @param	context	The enclosing subroutine context
 ************************************************************************************************/
bool PComp::forStatement(int level, SymbolTableEntry& context) {
	if (accept(Token::For)) {
		auto var = lookup(ts.current().string_value);
		expect(Token::Identifier);			// consume the identifier...
		if (var == symtbl.end())
			return true;					// give up if the identifier is undefined
		auto lhs = lvalueRef(level, var, false);
		emit(OpCode::DUP);					// dupliate the iternator reference

		expect(Token::In);					// "for" identifier "in" ...

		int inc = 1;						// default is to count up...
		if (accept(Token::Reverse))
			inc = -1;						// "for" identifier "in" "reverse"

		auto range = type(level, false);
		if (range == nullptr) {
			error("expected ordinal type");
			return true;					// give up...

		} else if (range->tclass() == TypeDesc::Array)
			range = range->itype();			// use the arrays index type...

		if (range == nullptr || !range->ordinal()) {
			error("expected ordinal type");
			return true;					// give up...
		}

		if (inc == 1)
			emit(OpCode::PUSH, 0, range->range().min());
		else
			emit(OpCode::PUSH, 0, range->range().max());
			
		emit(OpCode::ASSIGN, 0, lhs->size()); // initialize the iterator

		const auto cond_pc = code->size();	// Condition check

		emit(OpCode::DUP);					// dupliate the iternator reference, again
			emit(OpCode::EVAL, 0, lhs->size());	// evaluate the iterator and compare to the limit

		if (inc == 1) {
			emit(OpCode::PUSH, 0, range->range().max());
			emit(OpCode::LTE);					// is iterator <= the condition?

		} else {
			emit(OpCode::PUSH, 0, range->range().min());
			emit(OpCode::GTE);					// is iterator <= the condition?
		}

		const auto jmp_pc = emitJNEQI();	// Jump to end of statement if not

		expect(Token::Loop);				// ... loop statements...
		statementList(level, context);
		expect(Token::Endloop);				// ... endloop

		emit(OpCode::DUP);					// iterate; dupliate the iternator reference again
		emit(OpCode::DUP);					// and one more time
		emit(OpCode::EVAL, 0, 1);			// add (or subtract 1)
		emit(OpCode::PUSH, 0, inc);
		emit(OpCode::ADD);
		emit(OpCode::ASSIGN, 0, 1);			// update the iterator
		emitJumpI(cond_pc);					// jump back to check the condition again

		// pop the condition varaible off the stack...
		const auto done_pc = emit(OpCode::POP, 0, 1);

		if (verbose)
			cout << prefix(progName) << "patching address @ " << done_pc << " to " << code->size() << '\n';
		(*code)[jmp_pc].value = done_pc;

		return true;
	}

	return false;
}

/********************************************************************************************//**
 * return [ expression ];
 *
 * @param	level	The current block level.
 * @param	context	The enclosing subroutine context
 *
 * @return	true if a return statement was processed
 ************************************************************************************************/
bool PComp::returnStatement(int level, SymbolTableEntry& context) {
	if (accept(Token::Return)) {
		if (context.second.kind() == SymValue::Function) {
			emit(OpCode::PUSHVAR, 0, FrameRetVal);
			TDescPtr type = TypeDesc::newPointerDesc(context.second.type());

			// Emit the r-value and assign to the function return in the frame

			TDescPtr rtype = expression(level);
			assignPromote(type->base(), rtype);
			emit(OpCode::ASSIGN, 0, type->base()->size());
			emit(OpCode::RETF, 0, context.second.params().size());
			context.second.returned(true);
			return true;

		} else if (context.second.kind() == SymValue::Procedure) {
			emit(OpCode::RET);
			return true;

		} else {
			error("Attempt to return from a non-subroutine");
			return false;
		}
	}

	return false;
}

/********************************************************************************************//**
 * get ( expression )
 *
 * TOS
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::getStatement(int level) {
	TDescPtr type;
	if (accept(Token::OpenParen)) {			// process each expr-tuple..
		TDescPtr type = expression(level, true); // lvalue(s) to put
		emit(OpCode::PUSH, 0, type->size());
		expect(Token::CloseParen);

		switch(type->tclass()) {
		case TypeDesc::Array:
			switch(type->base()->tclass()) {
			case TypeDesc::Boolean:		emit(OpCode::GET, 0, Datum::Boolean);		break;
			case TypeDesc::Character:	emit(OpCode::GET, 0, Datum::Character);		break;
			case TypeDesc::Integer:		emit(OpCode::GET, 0, Datum::Integer);		break;
			case TypeDesc::Real:		emit(OpCode::GET, 0, Datum::Real);			break;
			default:	error("unsupported get parameter");
			}
			break;

		case TypeDesc::Boolean:		emit(OpCode::GET, 0, Datum::Boolean);			break;
		case TypeDesc::Character:	emit(OpCode::GET, 0, Datum::Character);			break;
		case TypeDesc::Integer:		emit(OpCode::GET, 0, Datum::Integer);			break;
		case TypeDesc::Real:		emit(OpCode::GET, 0, Datum::Real);				break;
		default:
			error("unsupported nullptr parameter");
		}
	}
}

/********************************************************************************************//**
 * put or put[ln]( [ expression [',' width [ ',' precision ]]] )
 *
 * Process put and putln parameters, up to, but not including, emitteing the final op-code.
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::put(int level) {
	const int defaultWidth = 0;
	const int defaultPrec = 0;

	if (accept(Token::OpenParen)) {			// process each expr-tuple..
		if (accept(Token::CloseParen)) {	// handle no parameters
			emit(OpCode::PUSH, 0, 0);		// size
			emit(OpCode::PUSH, 0, defaultWidth);
			emit(OpCode::PUSH, 0, defaultPrec);

		} else {
			auto expr = expression(level); 	// value(s) to put
			emit(OpCode::PUSH, 0, expr->size());

			if (accept(Token::Comma)) {		// [ ',' width [ ',' precision ]]
				ostringstream oss;

				auto width = expression(level);
				if (width->tclass() != TypeDesc::Integer) {
					oss << "expeced integer width parameter, got: " << width->tclass();
					error(oss.str());
				}

				if (accept(Token::Comma)) {	//	[ ',' precision ]
					auto prec = expression(level);
					if (prec->tclass() != TypeDesc::Integer) {
						oss << "expeced integer width parameter, got: " << width->tclass();
						error(oss.str());
					}

				} else						// push default precision
					emit(OpCode::PUSH, 0, defaultPrec);	// push default precision
						
			} else {						// push default width & precision
				emit(OpCode::PUSH, 0, defaultWidth);
				emit(OpCode::PUSH, 0, defaultPrec);
			}

			expect(Token::CloseParen);
		}
	}
}

/********************************************************************************************//**
 * put or putln( [ expression [',' width [ ',' precision ]]] )
 *
 * Process put and putln parameters, up to, but not including, emitteing the final op-code.
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::putStatement(int level) {
	put(level);
	emit(OpCode::PUT);
}

/********************************************************************************************//**
 * putln [ format-list ]
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::putLnStatement(int level) {
	put(level);
	emit(OpCode::PUTLN);
}

/********************************************************************************************//**
 * new(identifier)
 *
 * @note	Totken::New has already been consumed.
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::statementNew(int level) {
	expect(Token::OpenParen);

	if (expect(Token::Identifier, false)) {
		auto it = lookup(ts.current().string_value);
		next();								// consume the identifier

		TDescPtr tdesc = TypeDesc::newIntDesc();
		if (it != symtbl.end())
			tdesc = variable(level, it);

		if (tdesc->tclass() != TypeDesc::Pointer) {
			ostringstream oss;
			oss << "expected a pointer, got " << tdesc->tclass();
			error(oss.str());
		}

		// The object size must be a signed integer, else it will be interperted as an address
		assert(tdesc->size() < numeric_limits<int>::max());
		if (tdesc->size() > numeric_limits<int>::max())
			error("size of object exceeds MaxInt!");
		int n = static_cast<int>(tdesc->size());

		emit(OpCode::PUSH, 0, n);		// push the size of the id
		emit(OpCode::NEW);
		emit(OpCode::ASSIGN, 0, 1);

		expect(Token::CloseParen);
	}
}

/********************************************************************************************//**
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::statementProcs(int level) {
	ostringstream oss;

	if (accept(Token::Get))					// get '(' expr-tuple { ',' expr-tuple } ')'
		getStatement(level);

	else if (accept(Token::Put))					// put '(' expr-tuple { ',' expr-tuple } ')'
		putStatement(level);

	else if (accept(Token::Putln))			// putln [ '(' expr-tuple { ',' expr-tuple } ')' ]
		putLnStatement(level);

	else if (accept(Token::New))			// 'New (' id ')'
		statementNew(level);

	else if (accept(Token::Dispose)) {		// 'Dispose (' expr ')'
		expect(Token::OpenParen);
		auto tdesc = expression(level);
		if (tdesc->tclass() != TypeDesc::Pointer) {
			ostringstream oss;
			oss << "expected a pointer, got " << tdesc->tclass();
			error(oss.str());
		}
		emit(OpCode::DISPOSE);
		expect(Token::CloseParen);
		
	}

	// else: nothing
}

/********************************************************************************************//**
 * [ ident = expr						|
 *   if expr then stmt { else stmt }	|
 *   while expr do stmt					|
 *   repeat stmt until expr				|
 *   stmt-block ]
 *
 * @param	level	The current block level.
 * @param	context	The enclosing subroutine contest
 ************************************************************************************************/
void PComp::statement(int level, SymbolTableEntry& context) {
	if (identStatement(level)) 
		;
	else if (ifStatement(level, context))
		;
	else if (whileStatement(level, context))
		;
	else if (repeatStatement(level, context))
		;
	else if (forStatement(level, context))
		;
	else if (returnStatement(level, context))
		;
	else
		statementProcs(level);
}

/********************************************************************************************//**
 * const const-decl { ; const-decl } ;
 *
 * @note	Doesn't emit any code; just stores the named value in the symbol table.
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::constDeclList(int level) {
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
 * @param	var		True if type is a var parameter
 ************************************************************************************************/
void PComp::typeDecl(int level, bool var) {
	const string ident = nameDecl(level);			// Copy the identifier
	expect(Token::Is);								// Consume the "is"
	TDescPtr tdesc = type(level, var, ident);

	if (verbose)
		cout << prefix(progName) << "type " << ident << " = " << tdesc->tclass() << '\n';

	symtbl.insert(	{ ident, SymValue::makeType(level, tdesc)	} );
}

/********************************************************************************************//**
 * type type-decl { ; type-decl } ;
 *
 * @note	Doesn't emit any code; just stores the new type in the symbol table.
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::typeDeclList(int level) {
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
			typeDecl(level, false);

		} while (accept(Token::SemiColon));
	}
}

/********************************************************************************************//**
 * ident = const-expr ;
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::constDecl(int level) {
	auto ident = nameDecl(level);				// Copy the identifier
	expect(Token::EQU);							// Consume the "="
	auto value = constExpr();					// Get the constant value

	if (!value.first)
		error("expected a const-expression, got:", ts.current().string_value);

	TDescPtr type = value.second.kind() == Datum::Kind::Integer
		? TypeDesc::newIntDesc() : TypeDesc::newRealDesc();

	symtbl.insert(	{ ident, SymValue::makeConst(level, value.second, type) } );
	if (verbose)
		cout << prefix(progName) << "constDecl " << ident << ": " << level << ", " << value.second << '\n';
}

/********************************************************************************************//**
 * const" var-decl-lst ;
 *
 * @param	level	The current block level.
 * @return  Number of variables allocated before or after the activation frame.
 ************************************************************************************************/
int PComp::varDeclBlock(int level) {
	FieldVec	idents;							// vector of name/type pairs.

	if (accept(Token::VarDecl))
		varDeclList(level, false, "", idents);

	int sum = 0;								// Add up the size of every variable in the block
	for (const auto& id : idents)
		sum += id.type()->size();

	return sum;
}

/********************************************************************************************//**
 * var-decl { ';' var-decl }
 *
 * Allocate space on the stack for each variable, as a postivie offset from the end of current
 * activaction frame. Create a new entry in the symbol table, that notes the offset and data
 * type.
 *
 * @param			level		The current block level.
 * @param			params		True if processing formal parameters, false if variable declaractions. 
 * @param			idprefix	The identifier prefix
 * @param[in,out]	idents		Vector of identifer, kind pairs 
 *
 * @return  Offset of the next variable/parmeter from the current activation frame.
 ************************************************************************************************/
void PComp::varDeclList(int level, bool params, const string& idprefix, FieldVec& idents) {
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
		varDecl(level, false, idprefix, idents);
	} while (accept(Token::SemiColon));

	/*
	 * Set the starting offset from the activation frame. Locals have positive offsets, while
	 * parameters have negative offsets in reverse. 
	 */

	int dx = 0;
	if (params)
		for (const auto& id : idents)
			dx -= id.type()->size();

	for (const auto& id : idents) {				// install the results in the symbol table...
		if (verbose)
			cout	<< prefix(progName)		<< "var/param "
					<< id.name()			<< ": " 
					<< level 				<< ", "
			    	<< dx	 				<< ", " 
					<< id.type()->tclass()	<< '\n';

		auto range = symtbl.equal_range(id.name());	// Already defined?
		for (auto it = range.first; it != range.second; ++it)
			if (it->second.level() == level)
				error("previously defined", id.name());

		symtbl.insert( { id.name(), SymValue::makeVar(level, dx, id.type())	} );
		dx += id.type()->size();
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
 * @param	level		The current block level. 
 * @param	var			True if parameter is a var parameter
 * @param	idprefix	The identifer prefix
 *
 * @param[in,out]	idents	Vector of identifer, kind pairs
 ************************************************************************************************/
void PComp::varDecl(int level, bool var, const string& idprefix, FieldVec& idents) {
	vector<string> ids = identifierList(level, idprefix);
	expect(Token::Colon);
	TDescPtr tdesc = type(level, var, idprefix);
	for (auto& id : ids)
		idents.push_back({ id, tdesc });
}

/********************************************************************************************//**
 * identifier-lst : identifer { ',' identifier }
 * @param			level	The current block level. 
 * @param	idprefix	The identifer prefix
 *
 * @return	List of identifiers in the identifier-lst
 ************************************************************************************************/
vector<string> PComp::identifierList(int level, const string& idprefix) {
	vector<string> ids;

	do {
		ids.push_back(nameDecl(level, idprefix));
	} while (accept(Token::Comma));

	return ids;
}

/********************************************************************************************//**
 * type = simple-type | structured-type | pointer-type ;
 *
 * Previously defined types, as well as  built-in types such as "Integer" and "Real" will have
 * Token::Type, thus we only need to look for new type declaractions, e.g., "array...".
 *
 * @param	level		The current block level. 
 * @param	var			True if parameter is a var parameter
 * @param	idprefix	Optional identifier prefix. Defaults to "".
 *
 * @return the type description
 ************************************************************************************************/
TDescPtr PComp::type(int level, bool var, const string& idprefix) {
	TDescPtr tdesc = TypeDesc::newIntDesc();

	if (accept(Token::Identifier, false)) { 	// previously defined type-name
		const string id = ts.current().string_value;
		auto it = lookup(id);
		next();									// Consume the identifier...

		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected type, got ", id);

		else {
			tdesc = TypeDesc::clone(it->second.type());
			tdesc->ref(var);
		}

	} else if (accept(Token::Caret)) 			// Pointer type
		tdesc = TypeDesc::newPointerDesc(type(level, var, idprefix), var);

	else if ((tdesc = structuredType(level, idprefix, var)) != nullptr)
		;

	else 
		tdesc = simpleType(level, var);

	return tdesc;
}

/********************************************************************************************//**
 * simple-type = "real" | ordinal-type ;
 *
 * @note Needs to handle previously defined simple-types
 * 
 * @param	level	The current block level. 
 * @param	var		True if parameter is a var parameter
 *
 * @return the type description
 ************************************************************************************************/
TDescPtr PComp::simpleType(int level, bool var) {
	TDescPtr type;

	if (accept(Token::Identifier, false)) {		// Previously defined type, including real
		const string id = ts.current().string_value;
		next();									// Consume the identifier

		auto it = lookup(id);
		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected real, got ", id);

		else if (it->second.type()->tclass() != TypeDesc::Real)
			error("expected real, got ", id);

		else {
			type = it->second.type();
			type->ref(var);
		}

	} else 
		type = ordinalType(level, var);

	return type;
}

/********************************************************************************************//**
 * ordinal-type-lst: ordinal-type { ',' ordinal-type } ;
 *
 * @param	level	The current block level. 
 * @param	var		True if parameter is a var parameter
 *
 * @return type description.
 ************************************************************************************************/
TDescPtrVec PComp::ordinalTypeList(int level, bool var) {
	TDescPtrVec tdescs;

	do {
		tdescs.push_back(ordinalType(level, var));
	} while (accept(Token::Comma));

	return tdescs;
}

/********************************************************************************************//**
 * 'Boolean' | 'Integer' | 'Char' |
 * '(' identifier-list ')' | 'array' '[' simple-type-list ']' 'of' type ;
 *
 * @note	built-in ordinal types, such as "integer" or non-ordinal types, such as "real" are 
 * 			already in the type system, and thus discovered as an identifier w/SymValue::Type.
 *
 * @param	level	The current block level. 
 * @param	var		True if parameter is a var parameter
 *
 * @return type description if successful, null pointer otherwise
 ************************************************************************************************/
TDescPtr PComp::ordinalType(int level, bool var) {
	TDescPtr type;

	if (accept(Token::Identifier, false)) { 	// previously defined type-name
		const string id = ts.current().string_value;
		auto it = lookup(id);
		next();									// Consume the identifier...

		if (it == symtbl.end() || it->second.kind() != SymValue::Type)
			error("expected type, got ", id);

		else if (!it->second.type()->ordinal())
			error("expected ordinal type, got ", it->first);

		else {
			type = TypeDesc::clone(it->second.type());
			type->ref(var);
		}

	} else if (accept(Token::OpenParen)) {		// Enumeration
		FieldVec		enums;

		const auto ids = identifierList(level, "");
		Subrange r(0, ids.empty() ? 0 : ids.size()-1);
		expect(Token::CloseParen);

		// Create the type, excluding the fields (enumerations)
		type = TypeDesc::newEnumDesc(r, FieldVec(), var);

		int value = 0;							// Each enumeration gets a value...
		for (auto id : ids) {
			enums.push_back( { id, type } );
			symtbl.insert(	{ id, SymValue::makeConst(level, Datum(value), type) }	);
			if (verbose)
				cout << prefix(progName) << "enumeration '" << id << "' = " << value << ", " << level << '\n';
			++value;
		}

		type->fields(enums);

	} else 
		type = subRangeType(var);

	return type;
}

/********************************************************************************************//**
 * const-expr '..' const-expr
 *
 * @param	var		True if parameter is a var parameter
 *
 * @return type description if successful, null pointer otherwise
 ************************************************************************************************/
TDescPtr PComp::subRangeType(bool var) {
	TDescPtr type;

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

		Subrange r(minValue.second.integer(), maxValue.second.integer());
		type = TypeDesc::newIntDesc(r, var);
	}

	return type;
}

/********************************************************************************************//**
 * structured-type = "array" '[' ordinal-type-lst ']' "of" type | "record" field-lst 'end' ;
 *
 * @param	level	The current block level. 
 * @param	previx	Optional identifier prefix
 * @param	var		True if parameter is a var parameter
 *
 * @return type description if successful, null pointer otherwise
 ************************************************************************************************/
TDescPtr PComp::structuredType(int level, const string& idprefix, bool var) {
	TDescPtr tdesc = 0;

	if (accept(Token::Array)) {					// Array
		expect(Token::OpenBrkt);				// "["

		TDescPtr tp;
		TDescPtrVec indexes = ordinalTypeList(level, var);
		for (auto index : indexes) {
			const Subrange r = index->range();
			tdesc = TypeDesc::newArrayDesc(r.span(), r, index, TDescPtr(), var);
			if (tp == 0)						// Remember the first array type descriptior...
				tp = tdesc;

			else {								// Following indexes...
				tp->size(tp->size() * r.span());
				tp = tp->base();
			}
		}

		expect(Token::CloseBrkt);				// "] of"
		expect(Token::Of);

		tp->base(type(level, var, ""));			// Get the array's base type
		tp->size(tp->size() * tp->base()->size());

	} else if (accept(Token::Record)) {			// Record
		FieldVec	fields;
		fieldList(level, idprefix, fields);

		unsigned sum = 0;						// Calc the total size of the record
		for (auto& element : fields)
			sum += element.type()->size();

		tdesc = TypeDesc::newRcrdDesc(sum, fields, var);

		expect(Token::End);
	}

	return tdesc;
}

/********************************************************************************************//**
 * var-decl-list 
 *
 * @param			level		The current block level.
 * @param			idprefix	The identifier prefix
 * @param[in,out]	fields		Vector of identifer, kind pairs. identifier arn't decorated.
 *
 * @return  Offset of the next variable/parmeter from the current activicaqtion frame.
 ************************************************************************************************/
void PComp::fieldList(int level, const string& idprefix, FieldVec& fields) {
	varDeclList(level, false, idprefix, fields);

	for (auto& fld : fields) {					// trim off the prefix '.' from the identifiers
		size_t n = fld.name().find('.');
		if (n != string::npos && n < fld.name().size()) {
			string name = fld.name();
			fld.name(name.erase(0, n+1));
		}
	}
}

/********************************************************************************************//**
 * param-decl { ';' param-decl } ;
 * param-decl = [ 'var' ] identifier-lst : type ;
 *
 * Allocate space on the stack for each variable, as a postivie offset from the end of current
 * activaction frame. Create a new entry in the symbol table, that notes the offset and data
 * type.
 *
 * @param			level		The current block level.
 * @param			params		True if processing formal parameters, false if variable declaractions. 
 * @param			idprefix	The identifier prefix
 * @param[in,out]	idents		Vector of identifer, kind pairs 
 *
 * @return  Offset of the next variable/parmeter from the current activation frame.
 ************************************************************************************************/
void PComp::paramDeclList(
			int			level,
			bool		params,
	const	string&		idprefix,
			FieldVec&	idents)
{
	// Stops if the ';' if followd by any of hte following tokens
	static const Token::KindSet stops {
		Token::ProcDecl,
		Token::FuncDecl,
		Token::Begin,
		Token::CloseParen
	};

	bool var = false;
	do {
		if (oneOf(stops))
			break;								// No more variables...

		if (accept(Token::VarDecl))
			var = true;

		varDecl(level, var, idprefix, idents);

	} while (accept(Token::SemiColon));

	/*
	 * Set the starting offset from the activation frame. Locals have positive offsets, while
	 * parameters have negative offsets in reverse. 
	 */

	int dx = 0;
	if (params)
		for (const auto& id : idents)
			dx -= id.type()->size();

	for (const auto& id : idents) {				// install the results in the symbol table...
		if (verbose)
			cout << prefix(progName) 	<< "var/param "
				 << id.name() 			<< ": " 
				 << level 				<< ", "
			     << dx	 				<< ", " 
				 << id.type()->tclass()	<< ", "
				 << boolalpha			<< id.type()->ref()
				 << '\n';

		auto range = symtbl.equal_range(id.name());	// Already defined?
		for (auto it = range.first; it != range.second; ++it)
			if (it->second.level() == level)
				error("previously defined", id.name());

		symtbl.insert( { id.name(), SymValue::makeVar(level, dx, id.type())	} );
		dx += id.type()->size();
	}
}

/********************************************************************************************//**
 * Process the common subroutine delcaration for procedures and functions:
 *
 * ident [ '(' param-decl-lst ')' ] ;
 * param-decl-lst = param-decl { ';' param-decl } ;
 * param-decl = [ 'var' ] identifier-lst : type ;
 *
 * @param	level	The current block level.
 * @param 	kind	The type of subroutine, e.g., procedure or fuction
 * @return	subrountine's symbol table entry
 ************************************************************************************************/
SymbolTableIter PComp::subroutineDecl(int level, SymValue::Kind kind) {
	auto ident = nameDecl(level);			// insert the name into the symbol table
	SymbolTableIter	it = symtbl.insert( { ident, SymValue::makeSbr(kind, level)	} );
	if (verbose)
		cout << prefix(progName) << "subroutineDecl " << ident << ": " << level << ", 0\n";

	if (expect(Token::OpenParen)) {			// Process the formal arguments, if any...
		FieldVec	idents;					// vector of name/type pairs.

		// Note that the activation frame level is that of the *following* block!
		paramDeclList(level+1, true, "", idents);

		expect(Token::CloseParen);

		for (auto id : idents)
			it->second.params().push_back(id.type());
	}

	return it;
}

/********************************************************************************************//**
 * procedure ident [ ( var-decl-lst ) ] ; block ;
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::procDecl(int level) {
	SymbolTableIter it = subroutineDecl(level, SymValue::Procedure);
	expect(Token::Is);
	blockDecl(*it, level + 1, Token::Endproc);
	emit(OpCode::RET, 0, it->second.params().size());
}

/********************************************************************************************//**
 * function ident [ ( var-decl-lst ) ] : type  block-decl ;
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void PComp::funcDecl(int level) {
	SymbolTableIter it = subroutineDecl(level, SymValue::Function);
	expect(Token::Colon);
	it->second.type(type(level, false, ""));
	expect(Token::Is);
	blockDecl(*it, level + 1, Token::Endfunc);
	if (!it->second.returned())
		error("Funcation has no return statement");
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
void PComp::subDeclList(int level) {
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
 * @param	context		The blocks (procedures) symbol table entry value
 * @param	level		The current block level.
 * @param	end			End of block token
 *
 * @return 	Entry point address
 ************************************************************************************************/
size_t PComp::blockDecl(SymbolTableEntry& context, int level, Token::Kind end) {
	LogLevel lvl;

	constDeclList(level);						// declaractions...
	typeDeclList(level);
	auto dx = varDeclBlock(level);
	subDeclList(level);

	/* Block body
	 *
	 * Emit the block's prefix, saving and return its address, followed by the postfix. Omit the prefix
	 * if dx == 0 (the subroutine has zero locals).
	 */

	const size_t addr = dx > 0 ? emit(OpCode::ENTER, 0, dx) : code->size();
	context.second.value(Datum(addr));

	if (expect(Token::Begin)) {					// "begin" statements... "end"
		statementList(level, context);
		expect(end);
	}

	purge(level);								// Remove symbols only visible at this level

	return addr;
}

/********************************************************************************************//**
 ************************************************************************************************/
void PComp::progDecl() {
	constexpr int level = 0;
	next();										// Fetch the 1st token

	expect(Token::ProgDecl);					// Program heading...
	SymbolTableIter it = subroutineDecl(level, SymValue::Procedure);
	expect(Token::Is);

	// Emit a call to the main procedure, followed by a halt
	const auto call_pc = emitCallI(level, 0);
	emit(OpCode::HALT);

	// Emit the first block...
	const size_t addr = blockDecl(*it, level, Token::Endprog);
	emit(OpCode::RET, 0, it->second.params().size());

	if (verbose)
		cout << prefix(progName) << "patching call to program at " << call_pc << " to " << addr  << '\n';
	(*code)[call_pc].value = addr;
}

/********************************************************************************************//**
 ************************************************************************************************/
void PComp::run()								{	progDecl();	}

// public:

/********************************************************************************************//**
 * Construct a new compilier with the token stream initially bound to std::cin.
 ************************************************************************************************/
PComp::PComp() : Compilier () {
	TDescPtr boolean	= TypeDesc::newBoolDesc();
	TDescPtr character	= TypeDesc::newCharDesc();
	TDescPtr integer	= TypeDesc::newIntDesc();
	TDescPtr natural	= TypeDesc::newIntDesc(Subrange(0, TypeDesc::maxRange.max()));
	TDescPtr positive	= TypeDesc::newIntDesc(Subrange(1, TypeDesc::maxRange.max()));
	TDescPtr real		= TypeDesc::newRealDesc();

	// Insert built-in types into the symbol table

	symtbl.insert( { "boolean",		SymValue::makeType(0, boolean)					} );
	symtbl.insert( { "integer",		SymValue::makeType(0, integer)					} );
	symtbl.insert( { "real",		SymValue::makeType(0, real)						} );

	// Built-in subrange types

	symtbl.insert( { "character",	SymValue::makeType(0, character)				} );
	symtbl.insert( { "natural",		SymValue::makeType(0, natural) 					} );
	symtbl.insert( { "positive",	SymValue::makeType(0, positive) 				} );

	// Built-in constants into the symbol table; id, level (always zero), and value

	symtbl.insert({"true",   		SymValue::makeConst(0, Datum(true), boolean)	} );
	symtbl.insert({"false",  		SymValue::makeConst(0, Datum(false), boolean)	} );
	symtbl.insert({"nil",    		SymValue::makeConst(
										0,
										Datum(0),
										TypeDesc::newPointerDesc(integer))			} );
}

