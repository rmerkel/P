/********************************************************************************************//**
 * @file pinterp.cc
 *
 * The P machine; a P language interpreter in C++
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

#include "pinterp.h"

using namespace std;

// private:

/********************************************************************************************//**
 * Dump the current machine state
 ************************************************************************************************/
void PInterp::dump() {
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

/********************************************************************************************//**
 * Checks to see if any part of the specified memory range is invalid, i.e.,  between the top of
 * the stack and the start of the heap.
 *
 * @param	begin	Start of the memory range
 * @param 	end		One past the end of the memory range
 ************************************************************************************************/
bool PInterp::rangeCheck(size_t begin, size_t end) {
	assert (begin <= end);

	const size_t stackEnd = sp + 1;			// one past the top-of-stack
	const size_t heapBegin = heap.addr();
	const size_t heapEnd = heapBegin + heap.size();

	if (begin < stackEnd && end <= stackEnd)
		return true;						// Range is in the stack
	else if (begin >= heapBegin && begin < heapEnd && end >= heapBegin && end <= heapEnd)
		return true;						// Range is in the heap
	else
		return false;
}

// protected:

/********************************************************************************************//**
 * @param lvl Number of levels down
 * @return The base, lvl's down the stack
 ************************************************************************************************/
unsigned PInterp::base(unsigned lvl) {
	auto b = fp;
	for (; lvl > 0; --lvl)
		b = stack[b].natural();

	return b;
}

/********************************************************************************************//**
 * @return the top-of-stack
 ************************************************************************************************/
Datum PInterp::pop()	{
	if (sp == 0)
		throw Error(stackUnderflow, "stack underflow error");
	else
		return stack[sp--];
}

/********************************************************************************************//**
 * @param n	number of datums to pop off the stack
 ************************************************************************************************/
void PInterp::pop(unsigned n)	{
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");
	else
		sp -= n;
}

/********************************************************************************************//**
 * @param d	Datum to push on to the stack
 ************************************************************************************************/
void PInterp::push(Datum d) {
	if (sp >= heap.addr())
		throw Error(stackOverflow, "stack overflow error");
	else
		stack[++sp] = d;
}

/********************************************************************************************//**
 * @param 	nlevel	Set the subroutines frame base nlevel's down
 * @param 	addr 	The address of the subroutine.
 ************************************************************************************************/
void PInterp::call(int8_t nlevel, unsigned addr) {
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
void PInterp::ret() {
	sp = fp - 1; 					// "pop" the activaction frame
	pc = stack[fp + FrameRetAddr].natural();
	fp = stack[fp + FrameOldFp].natural();
	sp -= ir.addr.natural();		// Pop parameters, if any...
}

/********************************************************************************************//**
 * Unlink the stack frame, set the return address, and then push the function result
 ************************************************************************************************/
void PInterp::retf() {
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
void PInterp::write1(unsigned index) {
	const auto value = stack[index];
	const auto width = stack[index+1].natural();
	const auto prec = stack[index+2].natural();

	switch(value.kind()) {
	case Datum::Boolean:	cout << << (value == 0 ? "false" : "true");			break;
	case Datum::Character:	cout << setw(width) << value;						break;
	case Datum::Integer:	cout << setw(width) << setprecision(prec) << value;	break;
	case Datum::Real:
		if (prec == 0)
			cout << setw(width) << scientific << setprecision(6) << value;
		else
			cout << setw(width) << fixed << setprecision(prec) << value;
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
bool PInterp::write() {
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

	sp -= (nargs * tupleSz) + 1;	// consume args & the arg count

	return true;
}

/********************************************************************************************//**
 * Assigns N Datums, where N is ir.addr, off the stack.  Stack layout before;
 *
 * stack  | contants
 * ------ | -------------------------------
 * sp-N   | dst - start destination address
 * ------ | -------------------------------
 * sp-N-1 | value 1
 * ------ | -------------------------------
 * sp-N-2 | value 2
 * ------ | -------------------------------
 * ... 	  | ...
 * ------ | -------------------------------
 * sp+1   | value N-1
 * ------ | -------------------------------
 * sp     | value N
 *
 * Copies value 1..N to stack[dest, dest+N), then pops N Datums off of the stack.
 *
 * @param	n	Number of Datums to assign.
 ************************************************************************************************/
void PInterp::assign(unsigned n) {
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");

	size_t dst = stack[sp - ir.addr.natural()].natural();
	if (!rangeCheck(dst, dst + n))
		throw Error(stackUnderflow, "stack underflow error");

	lastWrite = dst + n - 1;

	size_t src = sp - n + 1;
	for (size_t i = 0; i < n; ++i)
		stack[dst++] = stack[src++];

	sp -= n + 1;				// 'pop' the stack, including dest addr
}

/********************************************************************************************//**
 * Evaluate N Datums, whose starting address is on the stop of the stack. Replaces the address
 * with the values.
 *
 * @param	n	Number of Datums to assign.
 ************************************************************************************************/
void PInterp::eval(unsigned n) {
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");

	unsigned dst = pop().natural();
	if (!rangeCheck(dst, dst + n))
		throw Error(stackUnderflow, "stack underflow error");

	for (unsigned i = 0; i < n; ++i)
		push(stack[dst++]);
}

/********************************************************************************************//**
 * Copy N Datums. Stack before;
 *
 * stack  | contants
 * ------ | ----------------------------------
 * sp+1   | dst - destination starting address
 * ------ | ----------------------------------
 * sp     | src - source starting address
 *
 ************************************************************************************************/
void PInterp::copy(unsigned n) {
	unsigned src  = pop().natural();
	if (!rangeCheck(src, src + n))
		throw Error(stackUnderflow, "stack underflow error");

	unsigned dst = pop().natural();
	if (!rangeCheck(dst, dst + n))
		throw Error(stackUnderflow, "stack underflow error");

	lastWrite = dst + n;
	for (unsigned i = 0; i < ir.addr.natural(); ++i)
		stack[dst++] = stack[src++];
}

/********************************************************************************************//**
 * @return Result::success or...
 ************************************************************************************************/
PInterp::Result PInterp::step() {
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

	case OpCode::POP:
		for (unsigned i = 0; i < ir.addr.natural(); ++i)
			pop();
		break;

	case OpCode::PUSH: 	push(ir.addr);											break;
	case OpCode::PUSHVAR: push(base(ir.level) + ir.addr.integer());				break;
	case OpCode::EVAL:	eval(ir.addr.natural());								break;
	case OpCode::ASSIGN: assign(ir.addr.natural());								break;
	case OpCode::COPY:	copy(ir.addr.natural());								break;

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
PInterp::Result PInterp::run() {
	if (verbose)
		cout << "Reg  Addr Value/Instr\n"
			 << "---------------------\n";

	if (verbose)
		heap.dump(cout);					// Dump the initial heap state...

	Result status = Result::success;
	try {
		do {
			if (pc >= code.size()) {
				cerr << "pc (" << pc << ") is out of range: [0.." << code.size() << ")!\n";
				status = Result::badFetch;

			} else {
				dump();							// Dump state and disasm the next instruction
				status = step();
			}
		} while (Result::success == status);

	} catch (Error& exp) {
		cerr << "runtime error @pc " << pc << ", sp " << sp << ": " << exp.what() << endl;
		return exp.result();
	}

	return status;
}

//public

/********************************************************************************************//**
 * Initialize the machine into a reset state with verbose == false. 
 *
 * @param stackSz	Size of the evaluation & call stack, in Datums.
 * @param fstoreSz	Size of the free store, in Datums.
 ************************************************************************************************/
PInterp::PInterp(unsigned stackSz, unsigned fstoreSz)
	:	stackSize{stackSz},
		stack(stackSize + fstoreSz, -1),
		heap(stackSz, fstoreSz),
		verbose(false),
		ncycles(0)
{
	reset();
}

/********************************************************************************************//**
 *	@param	prog	The program to run
 *	@param 	ver		True for verbose/debugging messages
 * 
 *  @return	The number of machine cycles run
 ************************************************************************************************/
PInterp::Result PInterp::operator()(const InstrVector& prog, bool ver) {
	verbose = ver;
	code = prog;

	reset();

	auto result = run();
	if (Result::halted == result)
		result = Result::success;			// halted is normal!

	return result;
}

/********************************************************************************************//**
 ************************************************************************************************/
void PInterp::reset() {
	pc = 0;

	fp = 0;											// Setup the initial activacation frame
	for (sp = fp; sp < FrameSize; ++sp)
		stack[sp] = 0;
	sp = fp + FrameSize - 1;

	ncycles = 0;
}

/********************************************************************************************//**
 * @return number of machien cycles run so far
 ************************************************************************************************/
size_t PInterp::cycles() const {
	return ncycles;
}

// operators

/********************************************************************************************//**
 * @brief PInterp::Result stream put operator
 *
 * Puts Result value on os
 *
 * @param	os		Stream to write value to 
 * @param	result	The value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const PInterp::Result& result) {
	switch (result) {
	case PInterp::success:			os << "success";				break;
	case PInterp::divideByZero:		os << "divide-by-zero";			break;
	case PInterp::badFetch:			os << "bad-fetch";				break;
	case PInterp::unknownInstr:		os << "unknown-instruction";	break;
	case PInterp::stackOverflow:	os << "stack overflow";			break;
	case PInterp::stackUnderflow:	os << "stack underflow";		break;
	case PInterp::freeStoreError:	os << "free-store error";		break;
	case PInterp::outOfRange:		os << "out-of-range";			break;
	case PInterp::halted:			os << "halted";					break;
	default:
		return os << "undefined result!";
		assert(false);
	}

	return os;
}

