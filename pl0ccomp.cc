/** @file pl0ccomp.cc
 *
 * PL0C Compiler implementation
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "pl0ccomp.h"
#include "pl0cinterp.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>

using namespace std;

namespace pl0c {
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
			oss << "Unknown token: '" << t.string_value << ", (0x" << hex << t.number_value << ")";
			error(oss.str());
			return next();
		}

		if (verbose)
			cout << progName << ": getting '" << Token::toString(t.kind) << "', " << t.string_value << ", " << t.number_value << "\n";

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
	 *	Appends (op, level, addr) on code, returning the new OpCodescode[] index (address)
	 *
	 *	@param	op		The pl0 instruction operation code
	 *	@param	level	The pl0 instruction block level value. Defaults to zero.
	 *	@param	addr	The pl0 instructions address/value. Defaults to zero.
	 *
	 *	@return The address (code[] index) of the new instruction.
	 */
	size_t Comp::emit(const OpCode op, int8_t level, Integer addr) {
		const int lvl = static_cast<int>(level);
		if (verbose)
			cout << progName << ": emitting " << code->size() << ": "
					<< OpCodeInfo::info(op).name() << " " << lvl << ", " << addr
					<< "\n";

		code->push_back({op, level, addr});
		indextbl.push_back(ts.lineNum);			// update the cross index

		return code->size() - 1;				// so it's the address of just emitted instruction
	}

	/**
	 * Writes a listing on the output stream
	 *
	 * @param name		Name of the source file
	 * @param source 	The source file stream
	 * @param out		The listing file stream
	 */
	 void Comp::listing(const string& name, istream& source, ostream& out) {
		string 		line;   					// Current source line
		unsigned 	linenum = 1;				// source line number
		unsigned 	addr = 0;					// code address (index)

		while (addr < indextbl.size()) {
			while (linenum <= indextbl[addr]) {	// Print lines that lead up to code[addr]...
				getline(source, line);	++linenum;
				cout << "# " << name << ", " << linenum << ": " << line << "\n" << internal;
			}

			disasm(out, addr, (*code)[addr]);	// Disasmble resulting instructions...
			while (linenum-1 == indextbl[++addr])
				disasm(out, addr, (*code)[addr]);
		}

		while (getline(source, line))			// Any lines following '.'...
			cout << "#" << name << ", " << linenum++ << ": " << line << "\n" << internal;

		out << endl;
	}

	 /**
	  *  Local variables have an offset from the *end* of the current stack frame (bp), while parameters
	  *  have a negative offset from the start of the frame. Offset locals by the size of the activation
	  *  frame.
	  *
	  *  @param	level	The current block level
	  *  @param	val		The variable symbol table entry
	  */
	 void Comp::varRef(int level, const SymValue& val) {
		const auto offset = val.value >= 0 ? val.value + FrameSize : val.value;
		emit(OpCode::pushVar, level - val.level, offset);
	 }

	/**
	 * Push a variable's value, a constant value, or invoke, and push the results, of a function.
	 *
	 * ident | ident "(" [ ident { "," ident } ] ")"
	 *
	 * @param	level	The current block level
	 */
	void Comp::identifier(int level) {
		const string name = ts.current().string_value;
		next();								// Consume the identifier

		auto range = symtbl.equal_range(name);
		if (range.first == range.second)
			error("Undefined identifier", name);

		else {
			auto closest = range.first;
			for (auto it = range.first; it != range.second; ++it)
				if (it->second.level > closest->second.level)
					closest = it;

			if (SymValue::constant == closest->second.kind)
				emit(OpCode::pushConst, 0, closest->second.value);

			else if (SymValue::identifier == closest->second.kind) {
				varRef(level, closest->second);
				emit(OpCode::eval);

			} else if (SymValue::function == closest->second.kind) {
				expect(Token::OpenParen);
				if (!accept(Token::CloseParen, false)) {
					do
						expression(level);
					while (accept(Token::Comma));
				}
				expect(Token::CloseParen);
				emit(OpCode::call, level - closest->second.level, closest->second.value);

			} else
				error("Unknown symbol", name);
		}
	}

	/**
	 * factor = ident | number | "{" expression "}" ;
	 *
	 * @param	level	The current block level.
	 */
	void Comp::factor(int level) {
		if (accept(Token::Identifier, false))
			identifier(level);

		else if (accept(Token::Number, false)) {
			emit(OpCode::pushConst, 0, ts.current().number_value);
			expect(Token::Number);

		} else if (accept(Token::OpenParen)) {
			expression(level);
			expect(Token::CloseParen);

		} else {
			error("factor: syntax error; expected ident | num | { expr }, but got:",
				Token::toString(current()));
			next();
		}
	}

	/**
	 * urary = [ ("+" | "-" | "!" | "~") ] fact
	 * @param	level	The current block level
	 */
	void Comp::unary(int level) {
		     if (accept(Token::Add))	{	factor(level);	/* ignore + */		}
		else if (accept(Token::Subtract))	{	factor(level);	emit(OpCode::neg);	}
		else if (accept(Token::Not))	{	factor(level);	emit(OpCode::Not);	}
		else if (accept(Token::Complament))	{	factor(level);	emit(OpCode::comp);	}
		else								factor(level);
	}

	/**
	 * term =  fact { ("*" | "/" | "%") fact } ;
	 *
	 * @param level	The current block level
	 */
	void Comp::term(int level) {
		unary(level);

		for (;;) {
				 if (accept(Token::Multiply))	{	unary(level);	emit(OpCode::mul);	}
			else if (accept(Token::Divide))	{	unary(level);	emit(OpCode::div);	}
			else if (accept(Token::Mod))	{	unary(level);	emit(OpCode::rem);	}
			else break;
		}
	}

	/**
	 * additive = term { ("+" | "-") term } ;
	 * @param	level	The curretn block level.
	 */
	void Comp::addExpr(int level) {
		term(level);

		for (;;) {
				 if	(accept(Token::Add)) 	{   term(level);	emit(OpCode::add);  }
			else if (accept(Token::Subtract)) 	{   term(level);	emit(OpCode::sub);  }
			else break;
		}
	}

	/**
	 * shift = additive { ("<<" | ">>") additive } ;
	 * @param	level	The current block level.
	 */
	void Comp::shiftExpr(int level) {
		addExpr(level);
		for (;;) {
				 if (accept(Token::ShiftL))	{	addExpr(level);	emit(OpCode::lshift);	}
			else if (accept(Token::ShiftR))	{	addExpr(level);	emit(OpCode::rshift);	}
			else break;
		}
	}

	/**
	 * expr = shift { ("|" | "&" | "^") shift } ;
	 *
	 * @param	level	The current block level.
	 */
	void Comp::expression(int level) {
		shiftExpr(level);

		for (;;) {
				 if (accept(Token::BitOR))	{	shiftExpr(level); emit(OpCode::bor);  }
			else if (accept(Token::BitAND))	{	shiftExpr(level); emit(OpCode::band); }
			else if (accept(Token::BitXOR))	{	shiftExpr(level); emit(OpCode::bxor); }
			else break;
		}
	}

	/**
	 * relational =  expr { ("="|"!="|"<"|"<="|">"|">=") expr } ;
	 * @param level	The current block level
	 */
	void Comp::relational(int level) {
		expression(level);

		for (;;) {
				 if (accept(Token::LTE)) 	{   expression(level);  emit(OpCode::lte);  }
			else if (accept(Token::LessThan)) 	{   expression(level);	emit(OpCode::lt);   }
			else if (accept(Token::GreaterThan))		{	expression(level);	emit(OpCode::gt);   }
			else if (accept(Token::GTE)) 	{	expression(level);	emit(OpCode::gte);  }
			else if (accept(Token::EQU)) 	{   expression(level);	emit(OpCode::equ);  }
			else if (accept(Token::NEQU)) 	{	expression(level);	emit(OpCode::neq);  }
			else break;
		}
	}

	/**
	 * cond =  relational { (|"||"|"&&") relational } ;
	 *
	 * @param	level	The current block level.
	 */
	void Comp::condition(int level) {
		relational(level);

		for (;;) {
				 if (accept(Token::OR)) 	{	relational(level);	emit(OpCode::lor);  }
			else if (accept(Token::AND)) 	{	relational(level);	emit(OpCode::land);	}
			else break;
		}
	}

	/**
	 * ident "=" expression
	 *
	 * @param	name	The identifier value
	 * @param	val		The identifiers symbol table entry value
	 * @param	level	The current block level.
	 */
	void Comp::assignStmt(const string& name, const SymValue& val, int level) {
		expression(level);

		switch(val.kind) {
		case SymValue::identifier:
			varRef(level, val);
			emit(OpCode::assign);
			break;

		case SymValue::function:					// Save value in the frame's retValue element
			emit(OpCode::pushVar, 0, FrameRetVal);
			emit(OpCode::assign);
			break;

		case SymValue::constant:
			error("Can't assign to a constant", name);
			break;

		case SymValue::proc:
			error("Can't assign to a procedure", name);
			break;

		default:
			assert(false);
		}
	}

	/**
	 *  "call" ident "(" [ expr  { "," expr }] ")"...
	 *
	 * @param	name	The identifier value
	 * @param	val		The identifiers symbol table entry value
	 * @param	level	The current block level
	 */
	void Comp::callStmt(const string& name, const SymValue& val, int level) {
		if (!accept(Token::CloseParen, false))
			do {									// [expr {, expr }]
				expression(level);
			} while (accept (Token::Comma));

		expect(Token::CloseParen);

		if (SymValue::proc != val.kind)
			error("Identifier is not a procedure", name);
		else
			emit(OpCode::call, level - val.level, val.value);
	}

	/**
	 * ident "=" expr | ident "(" [ ident { "," ident } ] ")"
	 *
	 * @param	level	The current block level
	 */
	void Comp::identStmt(int level) {
		// Save the identifier string before consuming it
		const string name = ts.current().string_value;
		accept(Token::Identifier);

		auto range = symtbl.equal_range(name);		// look it up....
		if (range.first == range.second)
			error("undefined variable", name);

		else {
			auto closest = range.first;				// Find the "closest" entry
			for (auto it = range.first; it != range.second; ++it)
				if (it->second.level > closest->second.level)
					closest = it;

			if (accept(Token::Assign))			// ident "=" expression
				assignStmt(closest->first, closest->second, level);

			else if (accept(Token::OpenParen))		// proc "(" ... ")"
				callStmt(closest->first, closest->second, level);

			else
				error("identifier is not a variable or a procedure", name);
		}
	}


	/**
	 * "while" condition "do" statement...
	 *
	 * @param	level	The current block level.
	 */
	 void Comp::whileStmt(int level) {
		const size_t cond_pc = code->size();			// Start of while condition
		condition(level);

		const size_t jmp_pc = emit(OpCode::jneq, 0, 0);	// jump if condition false
		expect(Token::Do);								// consume "do"
		statement(level);

		emit(OpCode::jump, 0, cond_pc);					// Jump back to conditon test

		if (verbose)
			cout << progName << ": patching address at " << jmp_pc << " to " << code->size() << "\n";
		(*code)[jmp_pc].addr = code->size();			// Patch jump on condition false instruction
	 }

	/**
	 *  "if" condition "then" statement1 [ "else" statement2 ]
	 */
	 void Comp::ifStmt(int level) {
		condition(level);

		const size_t jmp_pc = emit(OpCode::jneq, 0, 0);	// jump if condition false
		expect(Token::Then);							// Consume "then"
		statement(level);

		// Jump over else statement, but only if there is an else
		const bool Else = accept(Token::Else);
		size_t else_pc = 0;
		if (Else) else_pc = emit(OpCode::jump, 0, 0);

		if (verbose)
			cout << progName << ": patching address at " << jmp_pc << " to " << code->size() << "\n";
		(*code)[jmp_pc].addr = code->size();				// Patch jump on condition false instruction

		if (Else) {
			statement(level);

			if (verbose)
				cout << progName << ": patching address at " << else_pc << " to " << code->size() << "\n";
			(*code)[else_pc].addr = code->size();				// Patch jump on condition false instruction
		}
	 }

	 /**
	  * [ "repeat" stmt "until" cond ]
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
	void Comp::statement(int level) {
		if (accept(Token::Identifier, false)) 			// assignment or proc call
			identStmt(level);

		else if (accept(Token::Begin)) {				// begin ... end
			do {
				statement(level);
			} while (accept(Token::SemiColon));
			expect(Token::End);

		} else if (accept(Token::If)) 					// if condition...
			ifStmt(level);

		else if (accept(Token::While))					// "while" condition...
			whileStmt(level);

		else if (accept(Token::Repeat))
			repeatStmt(level);

		// else: nothing
	}

	/**
	 * [ const ident = number {, ident = number} ; ]
	 *
	 * @note Doesn't emit any code; just stores the named value in the symbol table.
	 *
	 * @param	level	The current block level.
	 */
	void Comp::constDecl(int level) {
		// Capture the identifier name before consuming it...
		const string name = ts.current().string_value;

		expect(Token::Identifier);						// Consume the identifier
		expect(Token::Assign);							// Consume the "="
		if (expect(Token::Number, false)) {
			auto number = ts.current().number_value;
			next();										// Consume the number

			auto range = symtbl.equal_range(name);
			for (auto it = range.first; it != range.second; ++it)
				if (it->second.level == level) {
					error("identifier has previously been defined", name);
					return;
				}

			symtbl.insert({ name, { SymValue::constant, level, number } });
			if (verbose)
				cout << progName << ": constDecl " << name << ": " << level << ", " << number << "\n";
		}
	}

	/**
	 * Allocate space on the stack for the variable and install it's offset from the block in the symbol
	 * table.
	 *
	 * @param	offset	Stack offset for the next varaible
	 * @param	level	The current block level.
	 *
	 * @return	Stack offset for the next varaible.
	 */
	int Comp::varDecl(int offset, int level) {
		const string name = ts.current().string_value;

		if (expect(Token::Identifier)) {
			auto range = symtbl.equal_range(name);
			for (auto it = range.first; it != range.second; ++it)
				if (it->second.level == level) {
					error("identifer has previously been defined", name);
					return offset;
				}

			symtbl.insert( { name, { SymValue::identifier, level, offset }} );
			if (verbose)
				cout << progName << ": varDecl " << name << ": " << level << ", " << offset << "\n";
			return offset + 1;
		}

		return offset;
	}

	/**
	 *   "procedure" ident "(" [ident {, "ident" }] ")" block ";"
	 * | "function"  ident "(" [ident {, "ident" }] ")" block ";"
	 *
	 * @param	level	The current block level.
	 */
	void Comp::subDecl(int level) {
		const	Token::Kind kind = current();	// proc or function decl
		next();									// consume token...

												// Save the identifier...
		const 	string name = ts.current().string_value;
				vector<string> args;			// formal arguments, if any

		if (expect(Token::Identifier)) {
			auto range = symtbl.equal_range(name);
			for (auto it = range.first; it != range.second; ++it)
				if (it->second.level == level)
					error("identifier has previously been defined", name);

			SymbolTable::iterator it;			// insert the new symbol...
			if (Token::Procedure == kind) {
				it = symtbl.insert( { name, { SymValue::proc, level, 0 }} );
				if (verbose)
					cout << progName << ": procDecl " << name << ": " << level << ", 0\n";

			} else {							// funcDecl
				it = symtbl.insert( { name, { SymValue::function, level, 0 }} );
				if (verbose)
					cout << progName << ": funcDecl " << name << ": " << level << ", 0\n";
			}

			expect(Token::OpenParen);				// procedure name "()"
			if (accept(Token::Identifier, false)) { // identifier {, identifier }
				int offset = 0;					// Record the arguments...
				do {
					args.push_back(ts.current().string_value);
					--offset;							// -1, -2, ..., -n
					accept(Token::Identifier);			// Consume the identifier
				} while (accept(Token::Comma));

				/**
				 * Add the arguments, with negative offsets from the block/frame, so
				 * that they have offsets of -n, ..., -2, -1. Note that their levels
				 * must be the same as the following block.
				 */
				for (auto& x : args) {
					const string& name = x;
					symtbl.insert( { name, { SymValue::identifier, level+1, offset++ }});
				}
			}

			expect(Token::CloseParen);
			block(it->second, level+1, args.size());
			expect(Token::SemiColon);	// procedure declarations end with a ';'!
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
	 * @param   nargs	The number of arguments, passed to the block, that must be
	 * 					popped on return.
	 */
	void Comp::block(SymValue& val, int level, unsigned nargs) {
		int		dx 		= 0;						// Variable offset from *end* of activation frame
		auto 	call_pc	= emit(OpCode::call, 0, 0);	// Addr to be patched below

		emit(OpCode::halt);

		if (accept(Token::Constant)) {				// const ident = number, ...
			do {
				constDecl(level);

			} while (accept(Token::Comma));
			expect(Token::SemiColon);
		}

		if (accept(Token::Variable)) {				// var ident, ...
			do {
				dx = varDecl(dx, level);

			} while (accept(Token::Comma));
			expect(Token::SemiColon);
		}

		while (accept(Token::Procedure, false) || accept(Token::Function, false))
			subDecl(level);

		/*
		 * Block body
		 *
		 * Emit the block prefix, and then set the blocks staring address, and
		 * patch the jump to it, followed by the postfix.
		 */

		auto addr = emit(OpCode::enter, 0, dx);		// prefix
		if (verbose)
			cout << progName << ": patching address at " << call_pc << " to " << addr  << "\n";
		(*code)[call_pc].addr = val.value = addr;

		statement(level);

		assert(nargs < numeric_limits<int>::max());	// postfix...
		if (SymValue::function == val.kind)
			emit(OpCode::reti, 0, nargs);			//	function...
		else
			emit(OpCode::ret, 0, nargs);			//	procedure...

		// Finally, remove symbols only visible in this level
		for (auto i = symtbl.begin(); i != symtbl.end(); ) {
			if (i->second.level == level) {
				if (verbose)
					cout << progName << ": purging "
					<< i->first << ": "
					<< SymValue::toString(i->second.kind) << ", " << static_cast<int>(i->second.level) << ", " << i->second.value
					<< " from the symbol table\n";
				i = symtbl.erase(i);

			} else
				++i;
		}
	}

	void Comp::run() {
		next();
		auto range = symtbl.equal_range("main");
		assert(range.first != range.second);

		block(range.first->second, 0, 0);
		expect(Token::Period);
	}

	// public:

	/**
	 * Construct a new compilier with the token stream initially bound to std::cin.
	 * @param	pName	The prefix string used by error and verbose/diagnostic messages.
	 */
	Comp::Comp(const string& pName) : progName {pName}, nErrors{0}, verbose {false}, ts{cin} {
		symtbl.insert({"main", { SymValue::proc, 0, 0 }});	// Install the "main" rountine declaraction
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
}
