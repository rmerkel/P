/** @file interp.cc
 *
 * Pascal-Lite interpreter in C++
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include <algorithm>
#include <cassert>
#include <cmath>
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
unsigned Interp::base(unsigned lvl) {
	auto b = fp;
	for (; lvl > 0; --lvl)
		b = stack[b].natural();

	return b;
}

/**
 * Make sure that there is room for at least sp+n elements in the stack.
 * @param	n	Maximum number of entries pushed down on the stack, if necessary, to make
 *  		    sufficient room for sp+n entries.
 */
void Interp::mkStackSpace(unsigned n) {
	while (stack.size() <= static_cast<unsigned> (sp + n))
		stack.push_back(-1);
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
void Interp::call(int8_t nlevel, unsigned addr) {
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
	pc = stack[fp + FrameRetAddr].natural();
	fp = stack[fp + FrameOldFp].natural();
	sp -= ir.addr.natural();		// Pop parameters, if any...
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
	auto prevPc = pc;				// The previous pc
	ir = code[pc++];				// Fetch next instruction...
	++ncycles;

	auto info = OpCodeInfo::info(ir.op);
	if (sp < info.nElements()) {
		cerr << "Out of bounds stack access @ pc (" << prevPc << "), sp == " << sp << "!\n";
		return Result::stackUnderflow;
	}

	Datum	rhand;						// right-hand side of a binary operation

	switch(ir.op) {
	case OpCode::ITOR:
		stack[sp] = stack[sp].integer() * 1.0;
		break;

	case OpCode::ITOR2:
		stack[sp-1] = stack[sp-1].integer() * 1.0;
		break;

	case OpCode::RTOI:	
		stack[sp] = static_cast<int>(round(stack[sp].real()));
		break;

	case OpCode::Neg:		stack[sp] = -stack[sp];					break;

	case OpCode::Add:		rhand = pop(); push(pop() + rhand);		break;
	case OpCode::Sub:		rhand = pop(); push(pop() - rhand); 	break;
	case OpCode::Mul:		rhand = pop(); push(pop() * rhand);		break;
	
	case OpCode::Div:
		rhand = pop();
		if ((rhand.kind() == Datum::Real && rhand.real() != 0) || rhand.integer() != 0)
			push(pop() / rhand);

		else {
			cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;
		}
		break;

	case OpCode::Rem:
		rhand = pop();
		if ((rhand.kind() == Datum::Real && rhand.real() != 0) || rhand.real() != 0)
			push(pop() % rhand);

		else {
			cerr << "attempt to divide by zero @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;
		}
		break;

	case OpCode::LT:		rhand = pop(); push(pop()  < rhand); 	break;
	case OpCode::LTE:   	rhand = pop(); push(pop() <= rhand); 	break;
	case OpCode::EQU:   	rhand = pop(); push(pop() == rhand); 	break;
	case OpCode::GTE:   	rhand = pop(); push(pop() >= rhand); 	break;
	case OpCode::GT:		rhand = pop(); push(pop()  > rhand); 	break;
	case OpCode::NEQU:   	rhand = pop(); push(pop() != rhand); 	break;
	case OpCode::LOR:   	rhand = pop(); push(pop() || rhand); 	break;
	case OpCode::LAND:  	rhand = pop(); push(pop() && rhand); 	break;
	case OpCode::Push: 		push(ir.addr);							break;

	case OpCode::PushVar:
		push(base(ir.level) + ir.addr.integer());
		break;

	case OpCode::Eval: {	auto ea = pop(); push(stack[ea.natural()]);	}
		break;

	case OpCode::Assign:
		rhand = pop();					// Save the value
		lastWrite = pop().natural();	// Save the effective address for dump()...
		stack[lastWrite] = rhand;
		break;

	case OpCode::Call: 		call(ir.level, ir.addr.natural());		break;
	case OpCode::Ret:   	ret();  								break;
	case OpCode::Retf: 		retf();									break;

	case OpCode::Enter:
		mkStackSpace(ir.addr.natural());
			sp+=ir.addr.natural();
			break;

	case OpCode::Jump:		pc = ir.addr.natural();					break;

	case OpCode::JNEQ:
		if (pop().integer() == 0)
			pc = ir.addr.natural();
		break;

	case OpCode::Halt:		return Result::halted;					break;

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

