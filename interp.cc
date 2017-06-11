/** @file interp.cc
 *
 * PL/0 interpreter in C++
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include <cassert>
#include <iomanip>
#include <iostream>
#include <vector>

#include "interp.h"

using namespace std;

// private:

/// Dump the current machine state
void Interp::dump() {

	cout << fixed;							// Use fixed format for floating point values;

	// Dump the last write
	if (lastWrite.valid())
		cout
			<< "    "
			<< setw(5)	<< lastWrite << ": "
			<< setw(10) << stack[lastWrite]
		<< std::endl;
	lastWrite.invalidate();

	if (!verbose) return;

	// Dump the current  activation frame...
	assert(sp >= fp);
	cout    << "fp: " 	<< setw(5) 	<< fp << ": "
			<< right 	<< setw(10) << stack[fp]
			<< endl;

	for (auto bl = fp+1; bl < sp; ++bl)
		cout
			<<	"    "	<< setw(5)	<< bl << ": "
			<< right << setw(10) 	<< stack[bl]
			<< endl;

	cout    << "sp: " 	<< setw(5) 	<< sp << ": "
			<< right << setw(10) 	<< stack[sp]
			<< endl;

	disasm(cout, pc, code[pc], "pc");

	cout << endl;
}

// protected:

/**
 * @param lvl Number of levels down
 * @return The base, lvl's down the stack
 */
Datum::Unsigned Interp::base(Datum::Unsigned lvl) {
	auto b = fp;
	for (; lvl > 0; --lvl)
		b = stack[b].uinteger();

	return b;
}

/**
 * Make sure that there is room for at least sp+n elements in the stack.
 * @param	n	Maximum number of entries pushed down on the stack, if necessary, to make
 *  		    sufficient room for sp+n entries.
 */
void Interp::mkStackSpace(Datum::Unsigned n) {
	while (stack.size() <= static_cast<unsigned> (sp + n))
		stack.push_back(Datum(-1));
}

/// @return the top-of-stack
Datum Interp::pop()					{	return stack[sp--];		}

/// @param d	Datum to push on to the stack
void Interp::push(Datum d) {
	mkStackSpace(1);
	stack[++sp] = d;
}

/**
 * @param 	nlevel	Set the subroutines frame base nlevel's down
 * @param 	addr 	The address of the subroutine.
 */
void Interp::call(int8_t nlevel, Datum::Unsigned addr) {
	mkStackSpace(FrameSize);

	const auto oldFp = fp;			// Save a copy before we modify it

	// Push a new activation frame block on the stack:

	push(base(nlevel));				//	FrameBase

	fp = sp;						// 	fp points to the start of the new frame

	push(oldFp);					//	FrameOldFp
	push(pc);						//	FrameRetAddr
	push(0u);						//	FrameRetVal

	pc = addr;
}

/**
 * Unlinks the stack frame, setting the return address as the next instruciton.
 */
void Interp::ret() {
	sp = fp - 1; 					// "pop" the activaction frame
	pc = stack[fp + FrameRetAddr].uinteger();
	fp = stack[fp + FrameOldFp].uinteger();
	sp -= ir.addr.uinteger();				// Pop parameters, if any...
}

/// Unlink the stack frame, set the return address, and then push the function result
void Interp::retf() {
	// Save the function result, unlink the stack frame, return the result
	auto temp = stack[fp + FrameRetVal];
	ret();
	push(temp);
}

/// @return Result::success or...
Interp::Result Interp::step() {
	auto prevPc = pc;					// The previous pc
	ir = code[pc++];					// Fetch next instruction...
	++ncycles;

	auto info = OpCodeInfo::info(ir.op);
	if (sp < info.nElements()) {
		cerr << "Out of bounds stack access @ pc (" << prevPc << "), sp == " << sp << "!\n";
		return Result::stackUnderflow;
	}

	Datum		rhand;					// righthand side of a binary operation

	switch(ir.op) {
	case OpCode::NOT:		stack[sp] = !stack[sp];					break;
	case OpCode::NEG:		stack[sp] = -stack[sp];					break;
	case OpCode::COMP:		stack[sp] = ~stack[sp]; 				break;
	case OpCode::RTOI:		stack[sp] = stack[sp].integer();		break;

	case OpCode::ADD:   	rhand = pop(); push(pop() + rhand); 	break;
	case OpCode::SUB:		rhand = pop(); push(pop() - rhand); 	break;
	case OpCode::MUL:		rhand = pop(); push(pop() * rhand);		break;
	case OpCode::DIV:
		if (Datum(0) != (rhand = pop()))
			push(pop() / rhand);

		else {
			cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;
		}
		break;

	case OpCode::REM:
		if (Datum(0) != (rhand = pop()))
			push(pop() % rhand);

		else {
			cerr << "attempt to divide by zero @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;
		}
		break;

	case OpCode::OR:	 	rhand = pop(); push(pop() | rhand);		break;
	case OpCode::AND:   	rhand = pop(); push(pop() & rhand); 	break;
	case OpCode::XOR:   	rhand = pop(); push(pop() ^ rhand); 	break;
	case OpCode::SHFTL: 	rhand = pop(); push(pop() << rhand); 	break;
	case OpCode::SHFTR:		rhand = pop(); push(pop() >> rhand); 	break;
	case OpCode::LTE:   	rhand = pop(); push(pop() <= rhand); 	break;
	case OpCode::LT:		rhand = pop(); push(pop()  < rhand); 	break;
	case OpCode::EQU:   	rhand = pop(); push(pop() == rhand); 	break;
	case OpCode::GT:		rhand = pop(); push(pop()  > rhand); 	break;
	case OpCode::GTE:   	rhand = pop(); push(pop() >= rhand); 	break;
	case OpCode::NEQU:   	rhand = pop(); push(pop() != rhand); 	break;
	case OpCode::LOR:   	rhand = pop(); push(pop() || rhand); 	break;
	case OpCode::LAND:  	rhand = pop(); push(pop() && rhand); 	break;
	case OpCode::PUSH: 		push(ir.addr);							break;
	case OpCode::PUSHVAR:	
		push(base(ir.level) + ir.addr.uinteger());
		break;

	case OpCode::EVAL:	{   
		auto ea = pop(); 
		push(stack[ea.uinteger()]);	
	}	break;

	case OpCode::ASSIGN: {
		const auto value = pop();
		lastWrite = pop().uinteger();
		stack[lastWrite] = value;
	}   break;

	case OpCode::CALL:  	call(ir.level, ir.addr.uinteger());		break;
	case OpCode::RET:   	ret();  								break;
	case OpCode::RETF: 		retf();									break;
	case OpCode::ENTER:
		mkStackSpace(ir.addr.uinteger()); 
		sp += ir.addr.uinteger();
		break;

	case OpCode::JUMP:  	pc = ir.addr.uinteger();				break;
	case OpCode::JFAIL:
		if (pop() == Datum(0)) 
			pc = ir.addr.uinteger();			
		break;

	case OpCode::HALT:		return Result::halted;					break;
	default:
		cerr 	<< "Unknown op code: " << OpCodeInfo::info(ir.op).name()
				<< " found at pc (" << prevPc << ")!\n" << endl;
		return Result::unknownInstr;
	};

	return Result::success;
}

/**
 *  @return	Result::success, or ...
 */
Interp::Result Interp::run() {
	if (verbose)
		cout << "Reg  Addr Value/Instr\n"
			 << "---------------------\n";

	Result status = Result::success;
	do {
		if (pc >= code.size()) {
			cerr << "pc (" << pc << ") is out of range: [0.." << code.size() << ")!\n";
			status = Result::badFetch;

		} else if (sp >= stack.size()) {
			cerr << "sp (" << sp << ") is out of range [0.." << stack.size() << ")!\n";
			status = Result::stackUnderflow;

		} else {
			dump();							// Dump state and disasm the next instruction
			status = step();
		}

	} while (Result::success == status);

	return status;
}

//public

/**
 *  Initialize the machine into a reset state with verbose == false.
 */
Interp::Interp() : stack(FrameSize), verbose(false), ncycles(0)	{
	reset();
}

/**
 *	@param	program	The program to run
 *	@param 	ver		True for verbose/debugging messages
 *  @return	The number of machine cycles run
 */
Interp::Result Interp::operator()(const InstrVector& program, bool ver) {
	verbose = ver;

	code = program;
	reset();

	auto result = run();
	if (Result::halted == result)
		result = Result::success;			// halted is normal!

	return result;
}

void Interp::reset() {
	pc = 0;

	fp = 0;									// Setup the initial activacation frame
	for (sp = 0; sp < FrameSize; ++sp)
		stack[sp] = 0;
	sp = stack.size() - 1;

	ncycles = 0;
}

/// @return number of machien cycles run so far
size_t Interp::cycles() const {
	return ncycles;
}

// public static

/**
 * @param	r	Result who's name we want
 * @return	r's name string.
 */
string Interp::toString(Result r) {
	switch (r) {
	case Result::success:			return "success";			break;
	case Result::divideByZero:		return "divideByZero";		break;
	case Result::badFetch:			return "badFetch";			break;
	case Result::unknownInstr:		return "unknownInstr";		break;
	case Result::stackOverflow:		return "stackOverflow";		break;
	case Result::stackUnderflow:	return "stackUnderflow";	break;
	case Result::halted:			return "halted";			break;
	default:						return "undefined error!";
	}
}

