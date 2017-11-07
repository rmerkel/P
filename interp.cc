/********************************************************************************************//**
 * @file interp.cc
 *
 * Pascal-lite interpreter in C++
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "interp.h"

using namespace std;

// private:

/********************************************************************************************//**
 * Dump the current machine state
 ************************************************************************************************/
void Interp::dump() {
	if (lastWrite.valid())				// dump the last write...
		cout << "    "
			 << setw(5)	<< lastWrite << ": "
			 << setw(10) << stack[lastWrite]
			 << std::endl;
	lastWrite.invalidate();

	if (!verbose) return;

	// Dump the current  activation frame...
	assert(sp >= fp);
	cout    << "fp: " 	<< setw(5)	<< fp << ": "
			<< right 	<< setw(10)	<< stack[fp]
			<< endl;

	for (auto bl = fp+1; bl < sp; ++bl)
		cout
			<<	"    "	<< setw(5)	<< bl << ": "
			<< right	<< setw(10) << stack[bl]
			<< endl;

	cout    << "sp: " 	<< setw(5) 	<< sp << ": "
			<< right	<< setw(10) << stack[sp]
			<< endl;

	disasm(cout, pc, code[pc], "pc");

	cout << endl;
}

// protected:

/********************************************************************************************//**
 * @param lvl Number of levels down
 * @return The base, lvl's down the stack
 ************************************************************************************************/
unsigned Interp::base(unsigned lvl) {
	auto b = fp;
	for (; lvl > 0; --lvl)
		b = stack[b].natural();

	return b;
}

/********************************************************************************************//**
 * @return the top-of-stack
 ************************************************************************************************/
Datum Interp::pop()					{	return stack[sp--];		}

/********************************************************************************************//**
 * @param d	Datum to push on to the stack
 ************************************************************************************************/
void Interp::push(Datum d) {
	stack[++sp] = d;
}

/********************************************************************************************//**
 * @param 	nlevel	Set the subroutines frame base nlevel's down
 * @param 	addr 	The address of the subroutine.
 ************************************************************************************************/
void Interp::call(int8_t nlevel, unsigned addr) {
	const auto oldFp = fp;			// Save a copy before we modify it

	// Push a new activation frame block on the stack:

	push(base(nlevel));				//	FrameBase

	fp = sp;						// 	fp points to the start of the new frame

	push(oldFp);					//	FrameOldFp
	push(pc);						//	FrameRetAddr
	push(0u);						//	FrameRetVal

	pc = addr;
}

/********************************************************************************************//**
 * Unlinks the stack frame, setting the return address as the next instruciton.
 ************************************************************************************************/
void Interp::ret() {
	sp = fp - 1; 					// "pop" the activaction frame
	pc = stack[fp + FrameRetAddr].natural();
	fp = stack[fp + FrameOldFp].natural();
	sp -= ir.addr.natural();		// Pop parameters, if any...
}

/********************************************************************************************//**
 * Unlink the stack frame, set the return address, and then push the function result
 ************************************************************************************************/
void Interp::retf() {
	// Save the function result, unlink the stack frame, return the result
	auto temp = stack[fp + FrameRetVal];
	ret();
	push(temp);
}

/********************************************************************************************//**
 * expr, width, precision is on the stack.
 * @param index	index into stack[] for the expr, width [, precision] tuple to print
 * @return false if width or precision is negative, true otherwise
 ************************************************************************************************/
void Interp::write1(unsigned index) {
	const auto value = stack[index];
	const auto width = stack[index+1].natural();
	const auto prec = stack[index+2].natural();

	switch(value.kind()) {
	case Datum::Real:
		if (prec == 0)
			cout << setw(width) << scientific << setprecision(6) << value;
		else
			cout << setw(width) << fixed << setprecision(prec) << value;
		break;

	case Datum::Integer:
		cout << setw(width) << setprecision(prec) << value;
		break;
	
	case Datum::Boolean:
		cout << setw(width) << value;
		break;

	default:
		cerr << "unknown datum type: " << static_cast<unsigned>(value.kind()) << endl;
		assert(false);
	}
}

/********************************************************************************************//**
 * TOS contains the number of elements, followed by that many expr, width, precision tuples.
 * @return false on stack underflow, true otherwise
 ************************************************************************************************/
bool Interp::write() {
	// TOS is the number of preceeding entries to print
	const auto nargs = stack[sp].natural();
	const unsigned tupleSz = 3;		// each parameter is a 3-tuple

	if (nargs > sp) {
		cerr << "Stack underflow @ pc " << pc << endl;
		return false;

	} else if (nargs > 0) {
		for (unsigned i = 0; i < nargs-1; ++i) {
			write1(sp - (nargs * tupleSz) + (i * tupleSz));
			cout << ' ';
		}
		write1(sp - tupleSz);
	}

	sp -= (nargs * tupleSz) + 1;	// consume args & the  arg count

	return true;
}

/********************************************************************************************//**
 * @return Result::success or...
 ************************************************************************************************/
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
	case OpCode::ITOR:	stack[sp] = stack[sp].integer() * 1.0;					break;
	case OpCode::ITOR2:	stack[sp-1] = stack[sp-1].integer() * 1.0;				break;
	case OpCode::ROUND:	stack[sp] = static_cast<int>(round(stack[sp].real()));	break;
	case OpCode::TRUNC:	stack[sp] = static_cast<int>(stack[sp].real());			break;
	case OpCode::ABS:
		if (stack[sp].kind() == Datum::Integer)
			stack[sp] = abs(stack[sp].integer());
		else
			stack[sp] = fabs(stack[sp].real());
		break;

	case OpCode::ATAN:
		if (stack[sp].kind() == Datum::Integer)
			stack[sp] = atan(stack[sp].integer());
		else
			stack[sp] = atan(stack[sp].real());
		break;

	case OpCode::EXP:
		if (stack[sp].kind() == Datum::Integer)
			stack[sp] = exp(stack[sp].integer());
		else
			stack[sp] = exp(stack[sp].real());
		break;

	case OpCode::LOG:
		rhand = pop();
		if ((rhand.kind() == Datum::Real && rhand.real() == 0) || (rhand.kind() == Datum::Integer && rhand.integer() == 0)) {
			cerr << "Attempt to take log(0) @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;

		} else if (rhand.kind() == Datum::Integer)
			push(log(rhand.integer()));
		else
			push(log(rhand.real()));
		break;

	case OpCode::DUP:	push(stack[sp]);										break;
	case OpCode::ODD:	stack[sp] = (stack[sp].natural() & 1) == 1;				break;
	case OpCode::PRED:
		if (stack[sp] <= ir.addr.integer())
			return Result::outOfRange;
		stack[sp] = stack[sp].integer() - 1;
		break;

	case OpCode::SIN:
		if (stack[sp].kind() == Datum::Integer)
			stack[sp] = sin(stack[sp].integer());
		else
			stack[sp] = sin(stack[sp].real());
		break;

	case OpCode::SQR:	// note that Datum lacks a *= operator
		if (stack[sp].kind() == Datum::Integer)
			stack[sp] = stack[sp].integer() * stack[sp].integer();
		else
			stack[sp] = stack[sp].real() * stack[sp].real();
		break;

	case OpCode::SQRT:
		if (stack[sp].kind() == Datum::Integer)
			stack[sp] = sqrt(stack[sp].integer());
		else
			stack[sp] = sqrt(stack[sp].real());
		break;

	case OpCode::SUCC:
		if (stack[sp] >= ir.addr.integer())
			return Result::outOfRange;
		stack[sp] = stack[sp].integer() + 1;
		break;


	case OpCode::WRITE:
		if (!write())
			return Result::stackUnderflow;
		break;
			
	case OpCode::WRITELN:
		if (!write())
			return Result::stackUnderflow;
		cout << '\n';
		break;

	case OpCode::NEW:
		push(heap.alloc(pop().natural()));
		if (verbose)
			heap.dump(cout);					// Dump the new heap state...
		break;

	case OpCode::DISPOSE:
		rhand = pop();
		if (!heap.free(rhand.natural())) {
			cerr << "Dispose of " << rhand << " failed!\n";
			return Result::freeStoreError;
		}
		if (verbose)
			heap.dump(cout);					// Dump the new heap state...
		break;

	case OpCode::NEG:	stack[sp] = -stack[sp];									break;
	case OpCode::ADD:	rhand = pop(); push(pop() + rhand);						break;
	case OpCode::SUB:	rhand = pop(); push(pop() - rhand); 					break;
	case OpCode::MUL:	rhand = pop(); push(pop() * rhand);						break;
	
	case OpCode::DIV:
		rhand = pop();
		if ((rhand.kind() == Datum::Real && rhand.real() == 0) || (rhand.kind() == Datum::Integer && rhand.integer() == 0)) {
			cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;

		} else
			push(pop() / rhand);
		break;

	case OpCode::REM:
		rhand = pop();
		if ((rhand.kind() == Datum::Real && rhand.real() == 0) || (rhand.kind() == Datum::Integer && rhand.integer() == 0)) {
			cerr << "attempt to divide by zero @ pc (" << prevPc << ")!\n";
			return Result::divideByZero;

		} else
			push(pop() % rhand);
		break;

	case OpCode::LT:	rhand = pop(); push(pop()  < rhand); 					break;
	case OpCode::LTE:   rhand = pop(); push(pop() <= rhand); 					break;
	case OpCode::EQU:   rhand = pop(); push(pop() == rhand); 					break;
	case OpCode::GTE:  	rhand = pop(); push(pop() >= rhand); 					break;
	case OpCode::GT:	rhand = pop(); push(pop()  > rhand); 					break;
	case OpCode::NEQU: 	rhand = pop(); push(pop() != rhand); 					break;

	case OpCode::LOR:  	rhand = pop(); push(pop() || rhand); 					break;
	case OpCode::LAND: 	rhand = pop(); push(pop() && rhand); 					break;
	case OpCode::LNOT:	stack[sp] = !stack[sp];									break;

	case OpCode::POP:	pop();													break;
	case OpCode::PUSH: 	push(ir.addr);											break;
	case OpCode::PUSHVAR: push(base(ir.level) + ir.addr.integer());				break;
	case OpCode::EVAL:	{ auto ea = pop(); push(stack[ea.natural()]); }			break;
	case OpCode::ASSIGN:
		rhand = pop();					// Save the value
		lastWrite = pop().natural();	// Save the effective address for dump()...
		stack[lastWrite] = rhand;
		break;

	case OpCode::CALL: 	call(ir.level, ir.addr.natural());						break;
	case OpCode::RET:   ret();  												break;
	case OpCode::RETF: 	retf();													break;

	case OpCode::ENTER: sp+=ir.addr.natural(); break;
	case OpCode::JUMP:	pc = ir.addr.natural();									break;
	case OpCode::JNEQ:
		if (pop().boolean() == false)
			pc = ir.addr.natural();
		break;

	case OpCode::LLIMIT:
		if (stack[sp] < ir.addr.integer())
			return Result::outOfRange;
		break;

	case OpCode::ULIMIT:
		if (stack[sp] > ir.addr.integer())
			return Result::outOfRange;
		break;

	case OpCode::HALT:	return Result::halted;									break;

	default:
		cerr 	<< "Unknown op-code: " << OpCodeInfo::info(ir.op).name()
				<< " found at pc (" << prevPc << ")!\n" << endl;
		return Result::unknownInstr;
	};

	return Result::success;
}

/********************************************************************************************//**
 *  @return	Result::success, or ...
 ************************************************************************************************/
Interp::Result Interp::run() {
	if (verbose)
		cout << "Reg  Addr Value/Instr\n"
			 << "---------------------\n";

	if (verbose)
		heap.dump(cout);					// Dump the initial heap state...

	Result status = Result::success;
	do {
		if (pc >= code.size()) {
			cerr << "pc (" << pc << ") is out of range: [0.." << code.size() << ")!\n";
			status = Result::badFetch;

		} else if (sp >= heap.addr()) {
			cerr << "sp (" << sp << ") is out of range [0.." << heap.addr() << ")!\n";
			status = Result::stackUnderflow;

		} else {
			dump();							// Dump state and disasm the next instruction
			status = step();
		}

	} while (Result::success == status);

	return status;
}

//public

/********************************************************************************************//**
 * Initialize the machine into a reset state with verbose == false. 
 *
 * @param stackSz	Size of the stack, in datums. Defaults to 1K.
 * @param fstoreSz	Size of the free store, in datums. Defaults to 3K
 ************************************************************************************************/
Interp::Interp(unsigned stackSz, unsigned fstoreSz)
	:	stackSize{stackSz},
		stack(stackSize + fstoreSz, -1),
		verbose(false), ncycles(0),
		heap(stackSz, fstoreSz)
{
	reset();
}

/********************************************************************************************//**
 *	@param	program	The program to run
 *	@param 	ver		True for verbose/debugging messages
 *  @return	The number of machine cycles run
 ************************************************************************************************/
Interp::Result Interp::operator()(const InstrVector& program, bool ver) {
	verbose = ver;

	code = program;
	reset();

	auto result = run();
	if (Result::halted == result)
		result = Result::success;			// halted is normal!

	return result;
}

/********************************************************************************************//**
 ************************************************************************************************/
void Interp::reset() {
	pc = 0;

	fp = 0;									// Setup the initial activacation frame
	for (sp = 0; sp < FrameSize; ++sp)
		stack[sp] = 0;
	sp = FrameSize - 1;

	ncycles = 0;
}

/********************************************************************************************//**
 * @return number of machien cycles run so far
 ************************************************************************************************/
size_t Interp::cycles() const {
	return ncycles;
}

// operators

/********************************************************************************************//**
 * @brief Interp::Result stream put operator
 *
 * Puts Result value on os
 *
 * @param	os		Stream to write value to 
 * @param	result	The value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Interp::Result& result) {
	switch (result) {
	case Interp::success:			os << "success";				break;
	case Interp::divideByZero:		os << "divide-by-zero";			break;
	case Interp::badFetch:			os << "bad-fetch";				break;
	case Interp::unknownInstr:		os << "unknown-instruction";	break;
	case Interp::stackOverflow:		os << "stack overflow";			break;
	case Interp::stackUnderflow:	os << "stack underflow";		break;
	case Interp::freeStoreError:	os << "free-store error";		break;
	case Interp::outOfRange:		os << "out-of-range";			break;
	case Interp::halted:			os << "halted";					break;
	default:
		return os << "undefined result!";
		assert(false);
	}

	return os;
}

