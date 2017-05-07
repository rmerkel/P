/** @file comp.cc
 *
 * PL0C Compiler implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "comp.h"
#include "interp.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>

using namespace std;

// class PL0CComp

// protected:

/**
 * Write a diagnostic on standard error output, incrementing the error count.
 * @param msg The error message
 */
void Comp::error(const std::string& msg) {
	cerr << progName << ": " << msg << " near line " << ts.lineNum << endl;
	++nErrors;
}

/**
 * Write a diagnostic in the form "msg 'name'", on standard error output, incrementing the error
 * count.
 * @param msg The error message
 * @param name Parameter to msg.
 */
void Comp::error(const std::string& msg, const std::string& name) {
	error (msg + " \'" + name + "\'");
}

/// @return The next token from the token stream
Token Comp::next() {
	const Token t = ts.get();

	if (Token::Unknown == t.kind) {
		ostringstream oss;
		oss << "Unknown token: '" << t.string_value << "', (0x" << hex << t.integer_value << ")";
		error(oss.str());
		return next();
	}

	if (verbose)
		cout << progName << ": getting '" << Token::toString(t.kind) << "', " << t.string_value << ", " << t.integer_value << "\n";

	return t;
}

/**
 * Returns true, and optionally consumes the current token, if the current tokens "kind" is equal to kind.
 *
 * @param	kind	The token Kind to accept.
 * @param	get		Consume the token, if true. Defaults true.
 *
 * @return	true if the current token is a kind.
 */
bool Comp::accept(Token::Kind kind, bool get) {
	if (current() == kind) {
		if (get) next();			// consume the token
		return true;
	}

	return false;
}

/**
 * Evaluate and return accept(kind, get). Generate an error if accept() returns false.
 *
 * @param	kind	The token Kind to accept.
 * @param	get		Consume the token, if true. Defaults true.
 *
 * @return	true if the current token is a k.
 */
bool Comp::expect(Token::Kind kind, bool get) {
	if (accept(kind, get)) return true;

	error("expected", Token::toString(kind) + "\' got \'" + Token::toString(current()));
	return false;
}

/**
 * @param set	Token kind set to test
 * @return true if current() is a member of set.
 */
bool Comp::oneOf(Token::KindSet set) {
	return set.end() != set.find(current());
}

/**
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
 */
size_t Comp::emit(const OpCode op, int8_t level, Datum addr) {
	const int lvl = static_cast<int>(level);
	if (verbose)
		cout << progName << ": emitting " << code->size() << ": "
				<< OpCodeInfo::info(op).name() << " " << lvl << ", " << addr.i
				<< "\n";

	code->push_back({op, level, addr});
	indextbl.push_back(ts.lineNum);			// update the cross index

	return code->size() - 1;				// so it's the address of just emitted instruction
}

/**
 * Convert stack operand to a real as necessary. 
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 */
void Comp::promote (Datum::Kind lhs, Datum::Kind rhs) {
	if (lhs == rhs)
		;				// nothing to do

	else if (Datum::Kind::Real == rhs)
		emit(OpCode::itor2);				// promote lhs (TOS-1) to a real

	else
		emit(OpCode::itor);					// promote rhs to a real
}

/**
 * Convert rhs of assign to real if necessary, or emit error would need to be converted to an integer. 
 * @param	lhs	The type of the left-hand-side
 * @param	rhs	The type of the right-hand-side 
 */
void Comp::assignPromote (Datum::Kind lhs, Datum::Kind rhs) {
	if (lhs == rhs)
		;				// nothing to do

	else if (Datum::Kind::Real == rhs) {
		error("rounding lhs to fit in an integer");
		emit(OpCode::rtoi);				// promote lhs (TOS-1) to a real
	}

	else
		emit(OpCode::itor);					// promote rhs to a real
}

/**
 * Uses the cross index to write a listing on the output stream
 *
 * @param name		Name of the source file
 * @param source 	The source file stream
 * @param out		The listing file stream
 */
 void Comp::listing(const string& name, istream& source, ostream& out) {
	string 			line;   				// Current source line
	unsigned 		linenum = 1;			// source line number
	Datum::Unsigned	addr = 0;				// code address (index)

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

/**
 * @param level  The block level
 */
void Comp::purge(int level) {
	for (auto i = symtbl.begin(); i != symtbl.end(); ) {
		if (i->second.level() == level) {
			if (verbose)
				cout << progName << ": purging "
					 << i->first << ": "
					 << SymValue::toString(i->second.kind()) << ", "
					 << static_cast<int>(i->second.level()) << ", "
					 << i->second.value().i
					 << " from the symbol table\n";
			i = symtbl.erase(i);

		} else
			++i;
	}
}

 /**
  * Local variables have an offset from the *end* of the current stack frame (bp), while parameters
  * have a negative offset from the *start* of the frame -- offset locals by the size of the
  * activation frame.
  *
  *  @param	level	The current block level
  *  @param	val		The variable symbol table entry
  *  @return		Data type
  */
 Datum::Kind Comp::varRef(int level, const SymValue& val) {
	const auto offset = val.value().i >= 0 ? val.value().i + FrameSize : val.value().i;
	emit(OpCode::pushVar, level - val.level(), offset);
	return val.value().k;
 }

/**
 * Push a variable's value, a constant value, or invoke, and push the results, of a function.
 *      ident | ident "(" [ ident { "," ident } ] ")"
 *
 * @param	level	The current block level 
 * @return	Data type 
 */
 Datum::Kind Comp::identifier(int level) {
	Datum::Kind	kind = Datum::Kind::Integer;	// Identifier type

	const string name = ts.current().string_value;
	expect(Token::Identifier);					// Consume the identifier

	auto range = symtbl.equal_range(name);
	if (range.first == range.second)
		error("Undefined identifier", name);

	else {
		auto closest = range.first;
		for (auto it = range.first; it != range.second; ++it)
			if (it->second.level() > closest->second.level())
				closest = it;

		switch (closest->second.kind()) {
		case SymValue::Kind::Constant:
			kind = closest->second.value().k;	// Assume it's the symbol's value type
			if (Datum::Kind::Integer == kind)
				emit(OpCode::pushi, 0, closest->second.value());
			else
				emit(OpCode::pushr, 0, closest->second.value());
			break;

		case SymValue::Kind::Variable:
			varRef(level, closest->second);
			kind = closest->second.type();
			emit(OpCode::eval);
			break;

		case SymValue::Kind::Function:
			callStmt(closest->first, closest->second, level);
			kind = closest->second.type();	// Use the function return type
			break;

		default:
			error("Identifier is not a constant, variable or function", name);
		}
	}

	return kind;
}

/** 
 *  factor = ident | number | "{" expression "}" ; 
 *"round" "(" expr ")"					| 
 * fact = ident                                 |
 *        ident "(" [ expr { "," expr } ")"   	|
 *  	  "round" "(" expr ")"					|
 *        number                                |
 *        "(" expr ")"
 *  	  ;
 * 
 * @param	level	The current block level.
 * @return	Data type 
 */
Datum::Kind Comp::factor(int level) {
	auto kind = Datum::Kind::Integer;	// factor data type

	if (accept(Token::Identifier, false))
		kind = identifier(level);

	else if (accept(Token::Round))  {	// round(expr) to an integer
		expect(Token::OpenParen);
		kind = expression(level);
		expect(Token::CloseParen);	
		if (Datum::Kind::Integer != kind) {
			emit(OpCode::rtoi);
			kind = Datum::Kind::Integer;
		}
		
	} else if (accept(Token::IntegerNum, false)) {
		emit(OpCode::pushi, 0, ts.current().integer_value);
		expect(Token::IntegerNum);
		
	} else if (accept(Token::RealNum, false)) {
		kind = Datum::Kind::Real;
		emit(OpCode::pushr, 0, ts.current().real_value);
		expect(Token::RealNum);

	} else if (accept(Token::OpenParen)) {
		kind = expression(level);
		expect(Token::CloseParen);

	} else {
		error("factor: syntax error; expected ident | num | { expr }, but got:",
			Token::toString(current()));
		next();
	}

	return kind;
}

/**
 * urary = [ ("+" | "-" | "!" | "~") ] fact
 * @param	level	The current block level 
 * @return	Data type 
 */
Datum::Kind Comp::unary(int level) {
	auto kind = Datum::Kind::Integer;	// factor data type

	if (accept(Token::Add)) 
		kind = factor(level);					/* ignore unary + */	

	else if (accept(Token::Subtract)) {
		if (Datum::Kind::Integer == (kind = factor(level)))
			emit(OpCode::negi);
		else
			emit(OpCode::negr);

	} else if (accept(Token::NOT)) {
		if (Datum::Kind::Integer == (kind = factor(level)))
			emit(OpCode::noti);
		else
			emit(OpCode::notr);

	} else if (accept(Token::Complament)) {
		if (Datum::Kind::Integer == (kind = factor(level)))
			emit(OpCode::comp);
		else
			error("unary: complement a Real");

	} else									
		kind = factor(level);

	return kind;
}

/**
 * term =  fact { ("*" | "/" | "%") fact } ;
 *
 * @param level	The current block level 
 * @return	Data type  
 */
Datum::Kind Comp::term(int level) {
	const auto lhs = unary(level);

	for (;;) {
		if (accept(Token::Multiply)) {
			const auto rhs = unary(level);	
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::muli);
			else
				emit(OpCode::mulr);

		} else if (accept(Token::Divide)) {
			const auto rhs = unary(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::divi);	
			else
				emit(OpCode::divr);

		} else if (accept(Token::Mod)) {
			const auto rhs = unary(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::remi);
			else
				emit(OpCode::remr);

		} else
			break;
	}

	return lhs;
}

/**
 * additive = term { ("+" | "-") term } ;
 * @param	level	The current block level. 
 * @return	Data type  
 */
Datum::Kind Comp::addExpr(int level) {
	const auto lhs = term(level);

	for (;;) {
		if (accept(Token::Add)) {
			const auto rhs =  term(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::addi);
			else
				emit(OpCode::addr);

		} else if (accept(Token::Subtract)) {
			const auto rhs = term(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::subi);
			else
				emit(OpCode::subr);

		} else
			break;
	}

	return lhs;
}

/**
 * shift = additive { ("<<" | ">>") additive } ;
 * @param	level	The current block level. 
 * @return	Data type. 
 */
Datum::Kind Comp::shiftExpr(int level) {
	const auto lhs = addExpr(level);

	for (;;) {
		 if (accept(Token::ShiftL)) {
			 const auto rhs = addExpr(level);

			 if (Datum::Kind::Integer != lhs || Datum::Kind::Integer != rhs)
				 error("Shift operator without integer operand(s)");
			 else 
				 emit(OpCode::lshift);

		 } else if (accept(Token::ShiftR)) {
			 const auto rhs = addExpr(level);

			 if (Datum::Kind::Integer != lhs || Datum::Kind::Integer != rhs)
				 error("Shift operator without integer operand(s)");
			 else 
				 emit(OpCode::rshift);	

		 } else
			 break;
	}

	return lhs;
}

/**
 * expr = shift { ("|" | "&" | "^") shift } ;
 *
 * @param	level	The current block level. 
 * @return	Data type. 
 */
Datum::Kind Comp::expression(int level) {
	const auto lhs = shiftExpr(level);

	for (;;) {
		if (accept(Token::BitOR)) {
			const auto rhs = shiftExpr(level);
			if (Datum::Kind::Integer != lhs || Datum::Kind::Integer != rhs)
				error("binary bit operation without integer operation");
			else
				emit(OpCode::bor);

		} else if (accept(Token::BitAND)) {
			const auto rhs = shiftExpr(level);
			if (Datum::Kind::Integer != lhs || Datum::Kind::Integer != rhs)
				error("binary bit operation without integer operation");
			else
				emit(OpCode::band); 

		} else if (accept(Token::BitXOR)) {
			const auto rhs = shiftExpr(level);
			if (Datum::Kind::Integer != lhs || Datum::Kind::Integer != rhs)
				error("binary bit operation without integer operation");
			else
				emit(OpCode::bxor); 

		} else
			break;
	}

	return lhs;
}

/**
 * relational =  expr { ("="|"!="|"<"|"<="|">"|">=") expr } ;
 * @param level	The current block level 
 * @return	Data type. 
 */
Datum::Kind Comp::relational(int level) {
	const auto lhs = expression(level);

	for (;;) {
		if (accept(Token::LTE)) {
			const auto rhs = expression(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs) 
				emit(OpCode::ltei);
			else
				emit(OpCode::lter);

		} else if (accept(Token::LT)) {
			const auto rhs = expression(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::lti);
			else
				emit(OpCode::ltr);

		} else if (accept(Token::GT)) {
			const auto rhs = expression(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::gti);
			else
				emit(OpCode::gtr);
			
		} else if (accept(Token::GTE)) {
			const auto rhs = expression(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::gtei);
			else
				emit(OpCode::gter);
			
		} else if (accept(Token::EQU)) {
			const auto rhs = expression(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::equi);
			else
				emit(OpCode::equr);
			
		} else if (accept(Token::NEQU)) {
			const auto rhs = expression(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == lhs)
				emit(OpCode::neqi);
			else
				emit(OpCode::neqr);

		} else
			break;
	}

	return lhs;
}

/**
 * cond =  relational { (|"||"|"&&") relational } ;
 *
 * @param	level	The current block level. 
 * @return	Data type. 
 */
Datum::Kind Comp::condition(int level) {
	const auto lhs = relational(level);

	for (;;) {
		if (accept(Token::OR)) {
			const auto rhs = relational(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == rhs)
				emit(OpCode::lori);
			else
				emit(OpCode::lorr);

		} else if (accept(Token::AND)) {
			const auto rhs = relational(level);
			promote(lhs, rhs);

			if (Datum::Kind::Integer == rhs)
				emit(OpCode::landi);
			else
				emit(OpCode::landr);

		} else
			break;
	}

	return lhs;
}

/**
 * ident "=" expression
 *
 * @param	name	The identifier value
 * @param	val		The identifiers symbol table entry value
 * @param	level	The current block level.
 */
void Comp::assignStmt(const string& name, const SymValue& val, int level) {
	const auto rhs = expression(level);

	switch(val.kind()) {
	case SymValue::Kind::Variable:
		assignPromote(val.type(), rhs);
		varRef(level, val);
		emit(OpCode::assign);
		break;

	case SymValue::Kind::Function:		// Save value in the frame's retValue element
		assignPromote(val.type(), rhs);
		emit(OpCode::pushVar, 0, FrameRetVal);
		emit(OpCode::assign);
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

/**
 * Call a function or procedure...
 *
 *      ident "(" [ expr  { "," expr }] ")"...
 *
 * @param	name	The identifier value
 * @param	val		The identifiers symbol table entry value
 * @param	level	The current block level 
 */
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

	emit(OpCode::call, level - val.level(), val.value());
}

/**
 *     ident "=" expr | ident "(" [ ident { "," ident } ] ")"
 *
 * @param	level	The current block level
 */
void Comp::identStmt(int level) {
	// Save the identifier string before consuming it
	const string name = ts.current().string_value;
	expect(Token::Identifier);

	auto range = symtbl.equal_range(name);		// look it up....
	if (range.first == range.second)
		error("undefined variable", name);

	else {
		auto closest = range.first;				// Find the "closest" entry
		for (auto it = range.first; it != range.second; ++it)
			if (it->second.level() > closest->second.level())
				closest = it;

		if (accept(Token::Assign))			// ident "=" expression
			assignStmt(closest->first, closest->second, level);

		else if (SymValue::Kind::Function == closest->second.kind())
			error("calling function without assignment");

		else
			callStmt(closest->first, closest->second, level);
	}
}


/**
 * "while" condition "do" statement...
 *
 * @param	level	The current block level.
 */
 void Comp::whileStmt(int level) {
	const auto cond_pc = code->size();	// Start of while condition
	condition(level);

	// jump if condition is false...
	const auto jmp_pc = emit(OpCode::jneq, 0, 0);
	expect(Token::Do);					// consume "do"
	statement(level);

	emit(OpCode::jump, 0, cond_pc);		// Jump back to condition test...

	if (verbose)
		cout << progName << ": patching address at " << jmp_pc << " to " << code->size() << "\n";
	(*code)[jmp_pc].addr = code->size(); 
 }

/**
 *  "if" condition "then" statement1 [ "else" statement2 ]
 */
 void Comp::ifStmt(int level) {
	condition(level);

	// Jump if conditon is false
	const size_t jmp_pc = emit(OpCode::jneq, 0, 0);
	expect(Token::Then);							// Consume "then"
	statement(level);

	// Jump over else statement, but only if there is an else
	const bool Else = accept(Token::Else);
	size_t else_pc = 0;
	if (Else) else_pc = emit(OpCode::jump, 0, 0);

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

 /**
  * "repeat" statement "until" condition
  *
  * @param	level 	The current block level
  */
 void Comp::repeatStmt(int level) {
	 const size_t loop_pc = code->size();			// jump here until condition fails
	 statement(level);
	 expect(Token::Until);
	 condition(level);
	 emit(OpCode::jneq, 0, loop_pc);
 }

/**
 * <"begin"> stmt { "," stmt } "end" ;
 *  @param	level		The current block level.
 */
void Comp::statementListTail(int level) {
	do {
		statement(level);
	} while (accept(Token::SemiColon));
	expect(Token::End);
}

/**
 * stmt =	[ ident ":=" expr
 * 		  	| "call" ident
 *          | "?" ident
 * 		  	| "!" expr
 *          | "begin" stmt {";" stmt } "end"
 *          | "if" cond "then" stmt { "else" stmt }
 *          | "while" cond "do" stmt ] ;identStmt
 *
 * @param	level	The current block level.
 */
void Comp::statement(int level) {
	if (accept(Token::Identifier, false)) 			// assignment or proc call
		identStmt(level);

	else if (accept(Token::Begin)) {				// begin ... end
		statementListTail(level);

	} else if (accept(Token::If)) 					// if condition...
		ifStmt(level);

	else if (accept(Token::While))					// "while" condition...
		whileStmt(level);

	else if (accept(Token::Repeat))
		repeatStmt(level);

	// else: nothing
}

/**
 * Scans and returns the next token in the stream which is expected to be an identifier. Checks to
 * see, and reports, if the identifier has already been delcared in this scope (level).
 * @param 	level	The current block level.
 * @return 	the next identifer in the token stream, "unknown" if the next token wasn't an
 *  		identifier.
 */
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

/**
 * ":"  "integer" | "real"
 * @return the datum type
 */
Datum::Kind Comp::typeDecl() {
	expect(Token::Colon);

	Datum::Kind kind;
		 if (accept(Token::Integer))	kind = Datum::Kind::Integer;
	else if (accept(Token::Real))		kind = Datum::Kind::Real;
	else {
		kind = Datum::Kind::Integer;
		error("expected 'integer' or 'real', got neither");
	}

	return kind;
}

/**
 * block-decl = [ "const" const-decl-blk ";" ]
 * const-decl-blk = const-decl-lst { ";" const-decl-lst ;
 *
 * @note	Doesn't emit any code; just stores the named value in the symbol table.
 * @param	level	The current block level.
 */
void Comp::constDeclBlock(int level) {
	// Stops if the ';' if followd by any of hte following tokens
	static const Token::KindSet stops {
		Token::VarDecl,
		Token::ProcDecl,
		Token::FuncDecl,
		Token::Begin
	};

	if (accept(Token::ConsDecl)) {
		do {
			if (oneOf(stops))
				break;							// No more constants...

			constDeclList(level);

		} while (accept(Token::SemiColon));
	}
}

/**
 * const-decl-lst = const-decl { "," const-decl } ;
 * const-decl =  ident "=" number | ident ;
 * @param	level	The current block level.
 */
void Comp::constDeclList(int level) {
	do {
		constDecl(level);
	} while (accept(Token::Comma));
}

/**
 * const-decl-lst = const-decl { "," const-decl } ;
 * const-decl =     ident "=" number | ident ;
 *
 * @param	level	The current block level.
 */
void Comp::constDecl(int level) {
	const auto ident = nameDecl(level);				// Copy the identifier

	expect(Token::Assign);							// Consume the "="
	if (accept(Token::IntegerNum, false)) {
		const auto number = ts.current().integer_value;
		next();										// Consume the number

		// Insert ident into the symbol table
		symtbl.insert(	{ ident, SymValue(level, number)	}	);
		if (verbose)
			cout << progName << ": constDecl " << ident << ": " << level << ", " << number << "\n";

	} else if (accept(Token::RealNum, false)) {
		const auto number = ts.current().real_value;
		next();										// Consume the number

		/// Insert ident into the symbol table
		symtbl.insert(	{	ident, SymValue(level, number)	}	);
		if (verbose)
			cout << progName << ": constDecl " << ident << ": " << level << ", " << number << "\n";

	} else {
		error("expected an interger or real literal, got neither", ts.current().string_value);
		next();
	}
}

/**
 * A varaiable declaration block;
 *     var-decl-blk = "const" var-decl-lst
 *     var-decl-lst = var-decl { ";" var-decl }
 *     var-decl =	  ident-list : type ;
 *     ident-list =   ident { "," ident } ;
 *     type =         "integer" | "real" ;
 *
 * @param	level	The current block level.
 *
 * @return  Number of variables allocated before or after the activation frame.
 */
int Comp::varDeclBlock(int level) {
	NameKindVec	idents;				// vector of name/type pairs.

	if (accept(Token::VarDecl))
		varDeclList(level, false, idents);

	return idents.size();
}

/**
 * A semicolon seperated list of variable declarations.
 *
 *     var-decl-list = var-decl { ";" var-decl }
 *     var-decl =	    ident-list : type ;
 *     ident-list =     ident { "," ident } ;
 *     type =           "integer" | "real" ;
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
 */
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
				 << Datum::toString(id.kind) << "\n";

		symtbl.insert( { id.name, SymValue(level, dx++, id.kind)	} );
	}
}

/**
 * A semicolon list of variable, or formal parameter declractions. Each declaraction starts
 * with a comma separated list of identifiers, followed by a colon followed by a type
 * specifier:
 *
 *     var-decl =	    ident-list : type ;
 *     ident-list =     ident { "," ident } ;
 *     type =           "integer" | "real" ;
 *
 * Allocate space on the stack for each variable, as a positive offset from the *end* of
 * current activaction frame; 0, 1, ..., n-1. Parameters, pushed by the caller, are identified
 * by negative indexes from *before the start* of the frame; -1, -2, ..., -n. Create a new
 * entry in the symbol table for either.
 *  
 * @param			level	The current block level. 
 * @param[in,out]	idents	Vector of identifer, kind pairs
 */
void Comp::varDecl(int level, NameKindVec& idents) {
	vector<string> indentifiers;

	// Find and append comma separated identifiers to the list...
	do {
		indentifiers.push_back(nameDecl(level));
	} while (accept(Token::Comma));

	const Datum::Kind kind = typeDecl();

	for (auto& id : indentifiers)
		idents.push_back({ id, kind });
}

/**
 * Process the common subroutine delcaration for procedures and functions:
 *
 * ident "(" [ name { "," name } ] ")" ...
 *
 * @param	level	The current block level.
 * @param 	kind	The type of subroutine, e.g., procedure or fuction
 * @return	subrountine's symbol table entry
 */
SymValue& Comp::subPrefixDecl(int level, SymValue::Kind kind) {
	SymbolTable::iterator	it;				// Will point to the new symbol table entry...

	const auto& ident = nameDecl(level);	// insert the name into the symbol table
	it = symtbl.insert( { ident, SymValue(kind, level)	} );
	if (verbose)
		cout << progName << ": subrountine-decl " << ident << ": " << level << ", 0\n";

	// Process the formal auguments, if any...
	expect(Token::OpenParen);

	// Note that the activation fram elevel is that of the *following* block!

	NameKindVec	idents;				// vector of name/type pairs.
	varDeclList(level+1, true, idents);
	expect(Token::CloseParen);

	for (auto id : idents)
		it->second.params().push_back(id.kind);

	return it->second;
}

/**
 * "procedure" ident "(" [ident {, "ident" }] ")" block ";
 * @param	level	The current block level.
 */
void Comp::procDecl(int level) {
	auto& val = subPrefixDecl(level, SymValue::Kind::Procedure);
	blockDecl(val, level + 1);
	expect(Token::SemiColon);				// procedure declarations end with a ';'!
}

/**
 * "function"  ident "(" [ident {, "ident" }] ")"  ":" type  block ";"
 * @param	level	The current block level.
 */
void Comp::funcDecl(int level) {
	auto& val = subPrefixDecl(level, SymValue::Kind::Function);
	val.type(typeDecl());
	blockDecl(val, level + 1);
	expect(Token::SemiColon);	// function declarations end with a ';'!
}

/**
 * Zero or more function and/or procedure delclaractions:
 *
 *     { proc-decl | funct-decl }
 *     proc-decl =      "procedure" ident param-decl-lst block-decl ";" ;
 *     func-decl =      "function"  ident param-decl-lst ":" type block-decl ";" ;
 *     type ; type =    "integer" | "real" ;
 *
 * @param level The current block level.
 */
void Comp::subrountineDecls(int level) {
	for (;;) {
		if (accept(Token::ProcDecl))
			procDecl(level);

		else if (accept(Token::FuncDecl))
			funcDecl(level);

		else
			break;
	}
}

/**
 * block = 	[ const ident = number {, ident = number} ";"]
 *         	[ var ident {, ident} ";" ]
 *         	{ procedure ident "(" [ ident { "," ident } ] ")" block ";"
 *         	 | function ident "(" [ ident { "," ident } ] ")" block ";" }
 *          stmt ;
 *
 * @param	val		The blocks (procedures) symbol table entry value
 * @param	level	The current block level.
 * @return 	Entry point address
 */
Datum::Unsigned Comp::blockDecl(SymValue& val, int level) {
	/*
	 * Delcaractions...
	 */

	constDeclBlock(level);
	auto dx = varDeclBlock(level);
	subrountineDecls(level);

	/* Block body
	 *
	 * Emit the block's prefix, saving and return its address, followed by the postfix. Omit the prefix
	 * if dx == 0 (the subroutine has zero locals.
	 */

	const auto addr = dx > 0 ? emit(OpCode::enter, 0, dx) : code->size();
	val.value(addr);

	if (expect(Token::Begin))					// "begin" statements... "end"
		statementListTail(level);

	// block postfix... TBD; emit reti or retr for functions!

	const auto sz = val.params().size();
	if (SymValue::Kind::Function == val.kind())
		emit(OpCode::retf, 0, sz);	// function...
	else
		emit(OpCode::ret, 0, sz);	// procedure...

	purge(level);								// Remove symbols only visible at this level

	return addr;
}

void Comp::run() {
	next();										// Fetch the 1st token
	auto range = symtbl.equal_range("main");
	assert(range.first != range.second);

	// Emit a call to the main procedure, followed by a halt
	const auto call_pc = emit(OpCode::call, 0, 0);
	emit(OpCode::halt);

	// emit the first block (block 0)

	const auto addr = blockDecl(range.first->second, 0);
	if (verbose)
		cout << progName << ": patching call to main at " << call_pc << " to " << addr  << "\n";

	(*code)[call_pc].addr = addr;
	expect(Token::Period);
}

// public:

/**
 * Construct a new compilier with the token stream initially bound to std::cin.
 * @param	pName	The prefix string used by error and verbose/diagnostic messages.
 */
Comp::Comp(const string& pName) : progName {pName}, nErrors{0}, verbose {false}, ts{cin} {
	symtbl.insert({"main", SymValue(SymValue::Kind::Procedure, 0)});	// Install the "main" rountine declaraction
}

/**
 * @param	inFile	The source file name, where "-" means the standard input stream
 * @param	prog	The generated machine code is appended here
 * @param	verb	Output verbose messages if true
 * @return	The number of errors encountered
 */
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
