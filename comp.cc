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
#include <vector>

using namespace std;

/************************************************************************************************
 * class PL0CComp
 *
 * private static :
 ************************************************************************************************/

// built-in types

/// Type descriptor for integers
ConstTDescPtr	Comp::intDesc = TDesc::newTDesc(
	TDesc::Kind::Integer,
	1,
	SubRange(numeric_limits<Integer>::min(), numeric_limits<Integer>::max()),
	"",
	FieldVec());

/// Type descriptor for reals
ConstTDescPtr Comp::realDesc = TDesc::newTDesc(
	TDesc::Kind::Real,
	1,
	SubRange(numeric_limits<Real>::min(), numeric_limits<Real>::max()),
	"",
	FieldVec());

// private:

/********************************************************************************************//**
 * Write a diagnostic on standard error output, incrementing the error count.
 * @param msg The error message
 ************************************************************************************************/
void Comp::error(const std::string& msg) {
	cerr << progName << ": " << msg << " near line " << ts.lineNum << endl;
	++nErrors;
}

/********************************************************************************************//**
 * Write a diagnostic in the form "msg 'name'", on standard error output, incrementing the error
 * count.
 * @param msg The error message
 * @param name Parameter to msg.
 ************************************************************************************************/
void Comp::error(const std::string& msg, const std::string& name) {
	error (msg + " \'" + name + "\'");
}

/********************************************************************************************//**
 * @return The next token from the token stream
 ************************************************************************************************/
Token Comp::next() {
	const Token t = ts.get();

	if (Token::Unknown == t.kind) {
		ostringstream oss;
		oss
			<< "Unknown token: '" << t.string_value
			<< "', (0x" << hex << t.integer_value << ")";
		error(oss.str());
		return next();
	}

	if (verbose)
		cout
			<< progName << ": getting '"
			<< Token::toString(t.kind) << "', "
			<< t.string_value << ", " << t.integer_value << "\n";

	return t;
}

/********************************************************************************************//**
 * Returns true, and optionally consumes the current token, if the current tokens "kind" is equal to kind.
 *
 * @param	kind	The token Kind to accept.
 * @param	get		Consume the token, if true. Defaults true.
 *
 * @return	true if the current token is a kind.
 ************************************************************************************************/
bool Comp::accept(Token::Kind kind, bool get) {
	if (current() == kind) {
		if (get) next();			// consume the token
		return true;
	}

	return false;
}

/********************************************************************************************//**
 * Evaluate and return accept(kind, get). Generate an error if accept() returns false.
 *
 * @param	kind	The token Kind to accept.
 * @param	get		Consume the token, if true. Defaults true.
 *
 * @return	true if the current token is a k.
 ************************************************************************************************/
bool Comp::expect(Token::Kind kind, bool get) {
	if (accept(kind, get)) return true;

	error("expected", Token::toString(kind) + "\' got \'" + Token::toString(current()));
	return false;
}

/********************************************************************************************//**
 * @param set	Token kind set to test
 * @return true if current() is a member of set.
 ************************************************************************************************/
bool Comp::oneOf(Token::KindSet set) {
	return set.end() != set.find(current());
}

/********************************************************************************************//**
 * Assembles op, level, addr into a new instruction, and then appends the instruciton on
 * the end of code[], returning it's address/index in code[].
 *
 * Side effect; updates the cross index for the listing.
 *
 * @param	op		The pl0 instruction operation code
 * @param	level	The pl0 instruction block level value. Defaults to zero.
 * @param	addr	The pl0 instructions address/value. Defaults to zero.
 *
 * @return The address (code[] index) of the new instruction.
 ************************************************************************************************/
size_t Comp::emit(const OpCode op, int8_t level, Datum addr) {
	const int lvl = static_cast<int>(level);
	if (verbose)
		cout
			<< progName << ": emitting " << code->size() << ": "
			<< OpCodeInfo::info(op).name() << " " << lvl << ", "
			<< addr.integer() << "\n";

	code->push_back({op, level, addr});
	indextbl.push_back(ts.lineNum);			// update the cross index

	return code->size() - 1;				// so it's the address of just emitted instruction
}

/********************************************************************************************//**
 * Convert stack operand to a real as necessary. 
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 * @return  The promoted to Datum kind
 ************************************************************************************************/
ConstTDescPtr Comp::promote (ConstTDescPtr lhs, ConstTDescPtr rhs) {
	if (lhs->kind() == rhs->kind())
		return lhs;							// nothing to do

	else if (rhs->kind() == TDesc::Real)
		emit(OpCode::ITOR2);				// promote lhs (TOS-1) to a real

	else
		emit(OpCode::ITOR);					// promote rhs to a real

	return realDesc;
}

/********************************************************************************************//**
 * Convert rhs of assign to real if necessary, or emit error would need to be
 * converted to an integer. 
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 ************************************************************************************************/
void Comp::assignPromote (ConstTDescPtr lhs, ConstTDescPtr rhs) {
	if (lhs->kind() == rhs->kind())
		;				// nothing to do

		else if (rhs->kind() == TDesc::Real) {
			error("rounding lhs to fit in an integer");
			emit(OpCode::RTOI);					// promote rhs to a integer

		} else
			emit(OpCode::ITOR);                 // promote rhs to a real
	}

	/********************************************************************************************//**
	 * Uses the cross index to write a listing on the output stream
	 *
	 * @param name		Name of the source file
	 * @param source 	The source file stream
	 * @param out		The listing file stream
	 ************************************************************************************************/
	 void Comp::listing(const string& name, istream& source, ostream& out) {
		string 		line;   				// Current source line
		unsigned 	linenum = 1;			// source line number
		Unsigned	addr = 0;				// code address (index)

		while (addr < indextbl.size()) {
			while (linenum <= indextbl[addr]) {	// Print lines that lead up to code[addr]...
				getline(source, line);	++linenum;
				cout << "# " << name << ", " << linenum << ": " << line << "\n" << internal;
			}

			disasm(out, addr, (*code)[addr]);	// Disasmble resulting instructions...
			while (linenum-1 == indextbl[++addr])
				disasm(out, addr, (*code)[addr]);
		}

		while (getline(source, line))			// Any lines following '.' ...
			cout << "#" << name << ", " << linenum++ << ": " << line << "\n" << internal;

		out << endl;
	}

	/********************************************************************************************//**
	 * @param level  The block level
	 ************************************************************************************************/
	void Comp::purge(int level) {
		for (auto i = symtbl.begin(); i != symtbl.end(); ) {
			if (i->second.level() == level) {
				if (verbose)
					cout << progName << ": purging "
						 << i->first << ": "
						 << SymValue::toString(i->second.kind()) << ", "
						 << static_cast<int>(i->second.level()) << ", "
						 << i->second.value().integer()
						 << " from the symbol table\n";
				i = symtbl.erase(i);

			} else
				++i;
		}
	}

	/********************************************************************************************//**
	 * Local variables have an offset from the *end* of the current stack frame
	 * (bp), while parameters have a negative offset from the *start* of the frame
	 *  -- offset locals by the size of the activation frame.
	 *
	 *  @param	level	The current block level
	 *  @param	val		The variable symbol table entry
	 *  @return			Data type
	 ************************************************************************************************/
	ConstTDescPtr Comp::emitVarRef(int level, const SymValue& val) {
	const auto offset = val.value().integer() >= 0 ? val.value().integer() + FrameSize : val.value().integer();
	emit(OpCode::PushVar, level - val.level(), offset);
	return val.type();
 }

/********************************************************************************************//**
 * Consume, and return the 'closest' (highest block level) identifer in the token stream...
 ************************************************************************************************/
SymbolTable::iterator Comp::identRef() {
	const string id = ts.current().string_value;	// Copy and, then 
	next();											// Consme the identifier...

	auto range = symtbl.equal_range(id);			// Should have already been defined...
	if (range.first == range.second) {
		error("Undefined identifier", id);
		return symtbl.end();

	} else {										// Find the closest...
		auto closest = range.first;
		for (auto it = closest; it != range.second; ++it)
			if (it->second.level() > closest->second.level())
				closest = it;

		return closest;
	}
}

/********************************************************************************************//**
 * Push a variable's value, a constant value, or invoke a function, and push the results,
 * of a function.
 *
 * ident | ident [ ( expr-lst ) ]
 *
 * @param	level	The current block level 
 * @return	Data type 
 ************************************************************************************************/
ConstTDescPtr Comp::identifier(int level) {
	auto type = intDesc;					// Identifiers type

	auto it = identRef();
	if (it != symtbl.end()) {
		switch (it->second.kind()) {
		case SymValue::Kind::Constant:
			type = it->second.type();
			emit(OpCode::Push, 0, it->second.value());
			break;

		case SymValue::Kind::Variable:
			type = it->second.type();
			emitVarRef(level, it->second);
			emit(OpCode::Eval);
			break;

		case SymValue::Kind::Function:
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
 * @param	level	The current block level.
 * @return	Data type 
 ************************************************************************************************/
ConstTDescPtr Comp::factor(int level) {
	auto type = intDesc;				// Factor data type

	if (accept(Token::Identifier, false))
		type = identifier(level);

	else if (accept(Token::Round))  {	// round(expr) to an integer
		expect(Token::OpenParen);
		type = expression(level);
		expect(Token::CloseParen);	

		if (type->kind() == TDesc::Real) {
			emit(OpCode::RTOI);
			type = intDesc;
		}
		
	} else if (accept(Token::IntegerNum, false)) {
		emit(OpCode::Push, 0, ts.current().integer_value);
		expect(Token::IntegerNum);
		
	} else if (accept(Token::RealNum, false)) {
		type = realDesc;
		emit(OpCode::Push, 0, ts.current().real_value);
		expect(Token::RealNum);

	} else if (accept(Token::OpenParen)) {
		type = expression(level);
		expect(Token::CloseParen);

	} else {
		error("factor: syntax error; expected ident | num | { expr }, but got:",
			Token::toString(current()));
		next();
	}

	return type;
}

/********************************************************************************************//**
 * fact { ( * | / | mod | and ) fact } ;
 *
 * @param level	The current block level 
 * @return	Data type  
 ************************************************************************************************/
ConstTDescPtr Comp::term(int level) {
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
 ************************************************************************************************/
/**
 * [ ( + | - ] term
 *
 * @param	level	The current block level 
 * @return	Data type 
 */
ConstTDescPtr Comp::unary(int level) {
	auto type = intDesc;					// Default factor data type

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
 * @param	level	The current block level. 
 * @return	Data type  
 ************************************************************************************************/
ConstTDescPtr Comp::simpleExpr(int level) {
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
 * @param level	The current block level 
 * @return	Data type. 
 ************************************************************************************************/
ConstTDescPtr Comp::expression(int level) {
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
		auto it = identRef();
		if (it != symtbl.end()) {
			if (it->second.kind() == SymValue::Kind::Constant)
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
 * ident = expression
 *
 * @param	name	The identifier value
 * @param	val		The identifiers symbol table entry value
 * @param	level	The current block level.
 ************************************************************************************************/
void Comp::assignStmt(const string& name, const SymValue& val, int level) {
	const auto rhs = expression(level);

	switch(val.kind()) {
	case SymValue::Kind::Variable:
		assignPromote(val.type(), rhs);
		emitVarRef(level, val);
		emit(OpCode::Assign);
		break;

	case SymValue::Kind::Function:		// Save value in the frame's retValue element
		assignPromote(val.type(), rhs);
		emit(OpCode::PushVar, 0, FrameRetVal);
		emit(OpCode::Assign);
		break;

	case SymValue::Kind::Constant:
		error("Can't assign to a constant", name);
		break;

	case SymValue::Kind::Procedure:
		error("Can't assign to a procedure", name);
		break;

	default:
		assert(false);
	}
}

/********************************************************************************************//**
 * Call a function or procedure...
 *
 * ident [ ( expr-list ) ]...
 *
 * @param	name	The identifier value
 * @param	val		The identifiers symbol table entry value
 * @param	level	The current block level 
 ************************************************************************************************/
void Comp::callStmt(const string& name, const SymValue& val, int level) {
	expect(Token::OpenParen);

	const auto& params = val.params();		// Formal parameter kinds
	unsigned nParams = 0;					// Count actual parameters
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
		oss << "passing " << nParams 
			<< " parameters where " << params.size()
			<< " where expected";
		error(oss.str());
	}

	if (SymValue::Kind::Procedure != val.kind() && SymValue::Kind::Function != val.kind())
		error("Identifier is not a function or procedure", name);

	emit(OpCode::Call, level - val.level(), val.value());
}

/********************************************************************************************//**
 * indentifier = expression | indentifier [ ( indentifier { , indentifier } ) ]
 *
 * @param	level	The current block level
 ************************************************************************************************/
void Comp::identStmt(int level) {
	auto it = identRef();

	if (accept(Token::Assign))			// ident "=" expression
		assignStmt(it->first, it->second, level);

	else if (SymValue::Kind::Function == it->second.kind())
		error("calling function without assignment", it->first);

	else
		callStmt(it->first, it->second, level);
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
 * [ ident = expr						|
 *   if expr then stmt { else stmt }	|
 *   while expr do stmt					|
 *   repeat stmt until expr				|
 *   stmt-block ]
 *
 * @param	level	The current block level.
 ************************************************************************************************/
void Comp::statement(int level) {
	if (accept(Token::Identifier, false)) 			// assignment or proc call
		identStmt(level);

	else if (accept(Token::Begin)) {				// begin ... end
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
 * Scans and returns the next token in the stream which is expected to be an identifier. Checks
 * to see, and reports, if the identifier has already been delcared in this scope (level).
 *
 * @param 	level	The current block level.
 * @return 	the next identifer in the token stream, "unknown" if the next token wasn't an
 *  		identifier.
 ************************************************************************************************/
const std::string Comp::nameDecl(int level) {
	const string id = ts.current().string_value;			// Copy the identifer

	if (expect(Token::Identifier)) {						// Consume the identifier
		auto range = symtbl.equal_range(id);				// Already defined?
		for (auto it = range.first; it != range.second; ++it) {
			if (it->second.level() == level) {
				error("identifier previously defined", id);
				break;
			}
		}

		return id;
	}

	return "unknown";
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

#if 0	// TBD...
	if (accept(Token::Integer))
		kind = Datum::Kind::Integer;
	else if (accept(Token::Real))
		kind = Datum::Kind::Real;
	else {
	}


	xxxxxxxxxxxxxxxx

	const auto value = constExpr();					// Get the constant value

	// Insert ident into the symbol table

	symtbl.insert(	{ ident, SymValue(level, value)	}	);
	if (verbose)
		cout << progName << ": constDecl " << ident << ": " << level << ", " << value << "\n";
#endif
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

	ConstTDescPtr type = value.kind() == Datum::Kind::Integer ? intDesc : realDesc;

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
				 << TDesc::toString(id.type->kind()) << "\n";

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
	vector<string> identifiers;

	// Find and append comma separated identifiers to the list...
	do {
		identifiers.push_back(nameDecl(level));
	} while (accept(Token::Comma));

	expect(Token::Colon);
	const auto desc = type();
	for (auto& id : identifiers)
		idents.push_back({ id, desc });
}

/********************************************************************************************//**
 * ":"  "integer" | "real"
 * @return the type description
 ************************************************************************************************/
ConstTDescPtr Comp::type() {
	auto type = intDesc;
	const string id = ts.current().string_value;	// Copy the type-identifer
	expect(Token::Type);

	auto range = symtbl.equal_range(id);			// Should have already been defined...
	if (range.first == range.second)
		error("Undefined type identifier", id);

	else {											// Find the closest...
		auto closest = range.first;
		for (auto it = closest; it != range.second; ++it)
			if (it->second.level() > closest->second.level())
				closest = it;

		type = closest->second.type();
	}

	return type;
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
		// Note that the activation fram elevel is that of the *following* block!

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
	auto& val = subPrefixDecl(level, SymValue::Kind::Procedure);
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
	auto& val = subPrefixDecl(level, SymValue::Kind::Function);
	expect(Token::Colon);
	val.type(type());
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
Unsigned Comp::blockDecl(SymValue& val, int level) {
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
	if (SymValue::Kind::Function == val.kind())
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
	auto& val = subPrefixDecl(level, SymValue::Kind::Procedure);
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
Comp::Comp(const string& pName) : progName {pName}, nErrors{0}, verbose {false}, ts{cin} {
	// insert builtin types into the symbol table
	symtbl.insert(	{"integer", SymValue(0, intDesc)	} );
	symtbl.insert(	{"real",	SymValue(0, realDesc)	} );
}

/********************************************************************************************//**
 * @param	inFile	The source file name, where "-" means the standard input stream
 * @param	prog	The generated machine code is appended here
 * @param	verb	Output verbose messages if true
 * @return	The number of errors encountered
 ************************************************************************************************/
unsigned Comp::operator()(const string& inFile, InstrVector& prog, bool verb) {
	code = &prog;
	verbose = verb;

	if ("-" == inFile)  {						// "-" means standard input
		ts.set_input(cin);
		run();

		// Just disasmemble as we can't rewind standard input!
		for (unsigned loc = 0; loc < code->size(); ++loc)
			disasm(cout, loc, (*code)[loc]);

	} else {
		ifstream ifile(inFile);
		if (!ifile.is_open())
			error("error opening source file", inFile);

		else {
			ts.set_input(ifile);
			run();

			ifile.close();						// Rewind the source (seekg(0) isn't working!)...
			ifile.open(inFile);
			listing(inFile, ifile, cout);		// 	create a listing...
		}
	}
	code = 0;

	return nErrors;
}

