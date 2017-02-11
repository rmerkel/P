/** @file pl0com.cc
 *
 * PL0Compilier implementation.
 */

#include "pl0com.h"
#include "pl0int.h"

#include <cassert>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace pl0;

// protected:

/** Write a error message
 *
 * Write a diagnostic to cerr, incrementing the error count.
 *
 * @param s The error message
 */
void PL0Compilier::error(const string& s) {
	cerr << progName << ": " << s << " near line " << ts.lineNum << endl;
	++nErrors;
}

/** Write a error message
 *
 * Write a diagnostic, in the form "s 't'", to cerr, incrementing the error count.
 *
 * @param s The error message
 * @param t Parameter to s.
 */
void PL0Compilier::error(const string& s, const string& t) {
	error (s + " \'" + t + "\'");
}

/// Returns a index in code[], one past the last instruction.
size_t PL0Compilier::cx() {
	assert(!code.empty());
	return code.size();
}

/** Get the next token
 *
 * @return Nnext token from the token stream
 */
Token PL0Compilier::next() {
	Token t = ts.get();

	if (verbose)
		cout << progName << ": getting " << String(t.kind) << ", " << t.string_value << ", " << t.number_value << "\n";

	return t;
}

/** Emit one pl0 instruction
 *
 *	Appends Instr(op, level, addr) onto code, and returns it's address.
 *
 *	@param	op		The pl0 instruction operation code
 *	@param	level	The pl0 instruction block level value. Defaults to zero.
 *	@param	addr	The pl0 instructions address/value. Defautls to zero.
 *
 *	@return The address (code[] index) of the new instruciton.
 */
size_t PL0Compilier::emit(const OpCode op, uint8_t level, Word addr) {
	if (verbose)
		cout << progName << ": emitting " << code.size() << ": "
				<< String(op) << " " << static_cast<unsigned>(level) << ", " << addr
				<< "\n";

	code.push_back({op, level, addr});
	return cx() - 1;		// so it's the addr of just emitted instruction
}

/** Accept the next token
 *
 * Returns true and optionally consumes the current token (ts) if it's a k.
 *
 * @param	k	The token Kind to accept.
 * @param	get	Consume the token, if true. Defaults true.
 *
 * @param	true if the current token is a k.
 */
bool PL0Compilier::accept(Kind k, bool get) {
	if (ts.current().kind == k) {
		if (get) next();			// consume the token
		return true;
	}

	return false;
}

/** Expect the next token Kind
 *
 * Evaluate and return accpet(k, get). If accept() returns false, generate an error message.
 *
 * @param	k	The token Kind to accept.
 * @param	get	Consume the token, if true. Defaults true.
 *
 * @param	true if the current token is a k.
 */
bool PL0Compilier::expect(Kind k, bool get) {
	if (accept(k, get)) return true;

	error("expected", String(k) + "\' got \'" + String(ts.current().kind));
	return false;
}

/** Factor identifier
 *
 * Push a variable or a constant value. The identifier must not be undefined.
 *
 * @param	level	The current block level
 */
void PL0Compilier::identifier(unsigned level) {
	const string& name = ts.current().string_value;
	const SymValue& v = ts.table[name];

	if (v.kind == Kind::undefined)
		error("undefined variable", name);

	else if (v.kind == Kind::constant)
		emit(OpCode::pushConst, 0, v.value);

	else									// mutable variable
		emit(OpCode::pushVar, level - v.level, v.value);

	expect(Kind::ident, true);				// Consume the identifier
}

/** Factor
 *
 * factor = ident | number | "{" expression "}" ;
 *
 * @param	level	The current block level.
 */
void PL0Compilier::factor(unsigned level) {
	if (accept(Kind::ident, false))
		identifier(level);

	else if (accept(Kind::number, false)) {
		emit(OpCode::pushConst, 0, ts.current().number_value);
		expect(Kind::number);

	} else if (accept(Kind::lp)) {
		expression(level);
		expect(Kind::rp);

	} else {
		error("factor: syntax error; expected ident | num | { expr }, but got:",
			String(ts.current().kind));
		next();
	}
}

/** Terminal
 *
 * term = fact { (*|/) fact } ;
 *
 * @param	level	The current block level.
 */
void PL0Compilier::terminal(unsigned level) {
	factor(level);

	for (Kind k = ts.current().kind; k == Kind::mul || k == Kind::div; k = ts.current().kind) {
		next();							// consume the token

		factor(level);
		Kind::mul == k ? emit(OpCode::Mul) : emit(OpCode::Div);
	}
}

/** Expresson
 *
 * expr = [ +|-] term { (+|-) term } ;
 *
 * @param	level	The current block level.
 */
void PL0Compilier::expression(unsigned level) {
	const Kind unary = ts.current().kind;	// unary [ +|-]?
	if (unary == Kind::plus || unary == Kind::minus)
		next();

	terminal(level);
	if (Kind::minus == unary)
		emit(OpCode::Neg);				// ignore unary +!

	for (Kind k = ts.current().kind; k == Kind::plus || k == Kind::minus; k = ts.current().kind) {
		next();						// consume the token

		terminal(level);
		Kind::plus == k ? emit(OpCode::Add) : emit(OpCode::Sub);
	}
}

/** Condition
 *
 * cond =  "odd" expr | expr ("="|"!="|"<"|"<="|">"|">=") expr ;
 *
 * @param	level	The current block level.
 */
void PL0Compilier::condition(unsigned level) {
	if (accept(Kind::Odd)) {
		expression(level);
		emit(OpCode::Odd);

	} else {
		expression(level);
		const Kind op = ts.current().kind;

		if (accept(Kind::lte)	||
			accept(Kind::lthan)	||
			accept(Kind::equal)	||
			accept(Kind::gthan)	||
			accept(Kind::gte)	||
			accept(Kind::nequal))
		{
			expression(level);

			switch(op) {
			case Kind::lte:		emit (OpCode::LTE);	break;
			case Kind::lthan:	emit (OpCode::LT);	break;
			case Kind::equal:	emit (OpCode::Equ);	break;
			case Kind::gthan:	emit (OpCode::GT);	break;
			case Kind::gte:		emit (OpCode::GTE);	break;
			case Kind::nequal:	emit (OpCode::Neq);	break;
			default:			assert(false);		// can't get here!
			}
		}
	}
}

/** Assignment statement
 *
 * ident "=" expression
 *
 * @param	level	The current block level.
 */
void PL0Compilier::assignStmt(unsigned level) {
	// Save the identifier string and loop it up in the symbol table, before consuming it
	const string name = ts.current().string_value;
	SymValue& v = ts.table[name];
	next();

	if (!expect(Kind::assign))
		error("expected assigment operator '='");

	if (v.kind == Kind::constant)
		error("attempt to modify constant variable", name);

	expression(level);

	emit(OpCode::Pop, level-v.level, v.value);
	v.kind = Kind::ident;			// no longer undefined!
}

/** While statement
 *
 * "while" condition "do" statement...
 *
 * @param	level	The current block level.
 */
 void PL0Compilier::whileStmt(unsigned level) {
	const size_t cond_pc = cx();				// Start of while condition
	condition(level);

	const size_t jmp_pc = emit(OpCode::Jne, 0, 0);		// jump if condition false
	expect(Kind::Do);							// consume "do"
	statement(level);

	emit(OpCode::Jump, 0, cond_pc);						// Jump back to conditon test

	if (verbose)
		cout << progName << ": patching address at " << jmp_pc << " to " << cx() << "\n";
	code[jmp_pc].addr = cx();					// Patch jump on condition false instruction
 }

/**
 */
 void PL0Compilier::ifStmt(unsigned level) {
	condition(level);

	const size_t jmp_pc = emit(OpCode::Jne, 0, 0);		// jump if condition false
	expect(Kind::Then);							// Consume "then"
	statement(level);

	if (verbose)
		cout << progName << ": patching address at " << jmp_pc << " to " << cx() << "\n";
	code[jmp_pc].addr = cx();					// Patch jump on condition false instruction

	// TBD: { "else" statement }
 }


/** Statement
 *
 * stmt =	[ ident ":=" expr
 * 		  	| "call" ident
 *          | "?" ident
 * 		  	| "!" expr
 *          | "begin" stmt {";" stmt } "end"
 *          | "if" cond "then" stmt { "else" stmt }
 *          | "while" cond "do" stmt ] ;
 *
 * @param	level	The current block level.
 */
void PL0Compilier::statement(unsigned level) {
	if (accept(Kind::ident, false)) 		// assignment
		assignStmt(level);

	else if (accept(Kind::Call)) {			// procedure call
		const string name = ts.current().string_value;
		SymValue& v = ts.table[name];
		expect(Kind::ident);
		emit(OpCode::Call, level-v.level, v.value);

	} else if (accept(Kind::Begin)) {		// begin ... end
		do {
			statement(level);
		} while (accept(Kind::scolon));
		expect(Kind::End);

	} else if (accept(Kind::If)) 			// if condition...
		ifStmt(level);

	else if (accept(Kind::While))			// "while" condition...
		whileStmt(level);

	// else: nothing
}

/** Constant declaration
 *
 * [ const ident = number {, ident = number} ; ]
 *
 * @note Doesn't emit any code; just stores the named value in the symbol table.
 *
 * @param	level	The current block level.
 */
void PL0Compilier::constDecl(unsigned level) {
	// Capture the identifier name before consuming it...
	const string name = ts.current().string_value;

	expect(Kind::ident);
	expect(Kind::equal);
	if (expect(Kind::number, false))
		ts.table[name] = { Kind::constant, level, ts.current().number_value };
	next();								// Consume the number
}


/** Variable declaration
 *
 * Allocate space on the stack for the variable and install it's offset from the block in the symbol
 * table.
 *
 * @param	level	The current block level.
 */
int PL0Compilier::varDecl(int offset, unsigned level) {
	const string name = ts.current().string_value;

	if (expect(Kind::ident)) {
		ts.table[name] = {	Kind::ident, level, offset	};
		return offset + 1;

	} else
		return offset;
}

/** Procedure declaration
 *
 * { "procedure" ident ";" block ";" }
 *
 * @param	level	The current block level.
 */
void PL0Compilier::procDecl(unsigned level) {
	const string name = ts.current().string_value;

	if (expect(Kind::ident))
		ts.table[name] = {	Kind::ident, level, 0	};

	expect(Kind::scolon);	// procedure "ident" ";"
	block(name, level+1);
	expect(Kind::scolon);	// prodecure declarations end with ';"
}

/** program block
 *
 * block = 	[ const ident = number {, ident = number} ;]
 *         	[ var ident {, ident} ;]
 *         	{ procedure ident ; block ; }
 *          stmt ;
 *
 * @param	level	The current block level.
 */
void PL0Compilier::block(const string&	name, unsigned level) {
	auto 	jmp_pc	= emit(OpCode::Jump, 0, 0);		// Addr to be patched below..
	int		dx		= 3;					// Variable offset from block/frame

	if (accept(Kind::ConstDecl)) {			// const ident = number, ...
		do {
			constDecl(level);

		} while (accept(Kind::comma));
		expect(Kind::scolon);
	}

	if (accept(Kind::VarDecl)) {			// var ident, ...
		do {
			dx = varDecl(dx, level);

		} while (accept(Kind::comma));
		expect(Kind::scolon);
	}

	while (accept(Kind::ProcDecl))			// procedure ident; ...
		procDecl(level);

	/*
	 * Block body
	 *
	 * Emit the block prefix, and then set the blocks staring address, and
	 * patch the jump to it
	 */

	auto addr = emit(OpCode::Enter, 0, dx);
	if (verbose) cout << progName << ": patching address at " << jmp_pc << " to " << addr  << "\n";
	code[jmp_pc].addr = ts.table[name].value = addr;
	statement(level);						// block body...
	emit(OpCode::Return);							// block postfix

	// Finally, remove symbols only visable to this level
	for (auto i = ts.table.begin(); i != ts.table.end(); )
		if (level > 0 && i->second.level == level && (Kind::ident == i->second.kind || Kind::constant == i->second.kind))
			i = ts.table.erase(i);
		else
			++i;
}

//public:

/// Constructor; construct a compilier; use pName for errors
PL0Compilier::PL0Compilier(const string& pName) : progName {pName}, nErrors{0}, verbose {false}, ts{cin} {
	ts.table["begin"].kind		= Kind::Begin;	// Install the keywords into the symbol table
	ts.table["end"].kind		= Kind::End;
	ts.table["const"].kind		= Kind::ConstDecl;
	ts.table["var"].kind		= Kind::VarDecl;
	ts.table["procedure"].kind	= Kind::ProcDecl;
	ts.table["call"].kind		= Kind::Call;
	ts.table["begin"].kind		= Kind::Begin;
	ts.table["end"].kind		= Kind::End;
	ts.table["if"].kind			= Kind::If;
	ts.table["then"].kind		= Kind::Then;
	ts.table["else"].kind		= Kind::Else;
	ts.table["while"].kind		= Kind::While;
	ts.table["do"].kind			= Kind::Do;
	ts.table["odd"].kind		= Kind::Odd;

	ts.table["main"].kind		= Kind::ident;		// Install the "main" rountine declaraction
}

/** Run the compilier
 *
 * @param	prog	The resultant program is copied here
 * @param	v		Output verbose messages if true
 *
 * @return	The number of errors encountered
 */
unsigned PL0Compilier::operator()(InstrVector& prog, bool v) {
	verbose = v;

	next();											// Load the token stream...
	block("main", 0);
	expect(Kind::period);

	if (verbose) {
		cout << endl;

		unsigned loc = 0;
		for (auto it : code)
			disasm("", loc++, it);
		cout << "\n";

		cout << setw(10) << "Name" << setw(10) << "Kind" << setw(6) << "Level" << setw(10) << "Value\n";
		cout << setw(10) << "----" << setw(10) << "----" << setw(6) << "-----" << setw(10) << "-----\n";

		// TBD: need a operator<< for SymValue!!!
		for (auto x : ts.table)
			cout	<< setw(10) << x.first
					<< setw(10) << String(x.second.kind)
					<< setw( 6) << x.second.level
					<< setw(10) << x.second.value
					<< "\n";
		cout << endl;
	}

	prog = code;
		
	return nErrors;
}

