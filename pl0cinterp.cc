/** @file pl0cinterp.cc
 *
 * PL/0 interpreter in C++
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "pl0cinterp.h"

using namespace std;

namespace pl0c {

	// private:

	/// Dump the current machine state
	void Interp::dump() {
		// Dump the last write
		if (lastWrite.valid())
			cout << "    " << setw(5) << lastWrite << ": " << setw(10) << stack[lastWrite] << std::endl;
		lastWrite.invalidate();

		if (!verbose) return;

		// Dump the current  activation frame...
		if (sp == -1) {								// Happens after return from the main procedure
			cout 	 <<	"sp:    "         << sp << ": " << right << setw(10) << stack[sp] << endl;
			cout 	 <<	"fp: " << setw(5) << fp << ": " << right << setw(10) << stack[fp] << endl;

		} else {
			assert(sp >= fp);
			cout     << "fp: " << setw(5) << fp << ": " << right << setw(10) << stack[fp] << endl;
			for (int bl = fp+1; bl < sp; ++bl)
				cout <<	"    " << setw(5) << bl << ": " << right << setw(10) << stack[bl] << endl;
			cout     << "sp: " << setw(5) << sp << ": " << right << setw(10) << stack[sp] << endl;
		}

		disasm(cout, pc, code[pc], "pc");

		cout << endl;
	}

	// protected:

	/**
	 * @param lvl Number of levels down
	 * @return The base, lvl's down the stack
	 */
	uint16_t Interp::base(uint16_t lvl) {
		uint16_t b = fp;
		for (; lvl > 0; --lvl)
			b = stack[b];

		return b;
	}

	/**
	 * Make sure that there is room for n elements in the stack.
	 * @param	n	Number of entries that will be pushed down on the stack
	 */
	void Interp::mkStackSpace(size_t n) {
		while (stack.size() <= static_cast<unsigned> (sp + n))
			stack.push_back(-1);
	}

	/// @return the top-of-stack
	Integer Interp::pop() {
		assert(sp >= 0);
		return stack[sp--];
	}

	/// @return the top-of-stack as an Unsigned
	Unsigned Interp::popUnsigned() {
		assert(sp >= 0);
		return static_cast<Unsigned>(stack[sp--]);
	}

	/// @param i	Value to push on to the stack
	void Interp::push(Integer i) {
		mkStackSpace(1);
		stack[++sp] = i;
	}

	/// @param	u	Value to push on to the stack
	void Interp::push(Unsigned u) {
		mkStackSpace(1);
		stack[++sp] = static_cast<Integer>(u);
	}

	/**
	 * @param 	nlevel	Set the subroutines frame base nlevel's down
	 * @param 	addr 	The address of the subroutine.
	 */
	void Interp::call(int8_t nlevel, Integer addr) {
		mkStackSpace(FrameSize);

		const auto oldFp = fp;			// Save a copy before we modify it

		// Push a new activation frame block on the stack:

		push(base(nlevel));				//	FrameBase

		fp = sp;						// 	fp points to the start of the new frame

		push(oldFp);					//	FrameOldFp
		push(pc);						//	FrameRetAddr
		push(0);						//	FrameRetVal

		pc = addr;
	}

	/**
	 * Unlinks the stack frame, setting the return address as the next instruciton.
	 */
	void Interp::ret() {
		sp = fp - 1; 					// "pop" the activaction frame
		pc = stack[fp + FrameRetAddr];
		fp = stack[fp + FrameOldFp];
		sp -= ir.addr;					// Pop parameters, if any...
	}

	/// Unlink the stack frame, set the return address, and then push the function result
	void Interp::reti() {
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

		Integer		rhand;					// righthand side of a binary operation
		Unsigned	urhand;					// 		"		 of a unsigned op

		switch(ir.op) {
		case OpCode::Not:		stack[sp] = !stack[sp];					break;
		case OpCode::neg:		stack[sp] = -stack[sp];					break;
		case OpCode::comp:
			stack[sp] = static_cast<Unsigned>(~static_cast<Unsigned>(stack[sp]));
			break;
		case OpCode::add:   	rhand = pop(); push(pop() + rhand); 	break;
		case OpCode::sub:		rhand = pop(); push(pop() - rhand); 	break;
		case OpCode::mul:		rhand = pop(); push(pop() * rhand); 	break;
		case OpCode::div:
			if (0 != (rhand = pop()))	push(pop() / rhand);
			else {
				cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
				return Result::divideByZero;
			}
			break;
		case OpCode::rem:
			if (0 != (rhand = pop()))	push(pop() % rhand);
			else {
				cerr << "attempt to divide by zero @ pc (" << prevPc << ")!\n";
				return Result::divideByZero;
			}
			break;
		case OpCode::bor:	 	urhand = popUnsigned(); push(popUnsigned()  | urhand);	break;
		case OpCode::band:   	urhand = popUnsigned(); push(popUnsigned()  & urhand);	break;
		case OpCode::bxor:   	urhand = popUnsigned(); push(popUnsigned()  ^ urhand);	break;
		case OpCode::lshift: 	urhand = popUnsigned(); push(popUnsigned() << urhand);	break;
		case OpCode::rshift: 	urhand = popUnsigned(); push(popUnsigned() >> urhand);	break;

		case OpCode::lte:   	rhand = pop(); push(pop() <= rhand);	break;
		case OpCode::lt:		rhand = pop(); push(pop()  < rhand);	break;
		case OpCode::equ:   	rhand = pop(); push(pop() == rhand);	break;
		case OpCode::gt:		rhand = pop(); push(pop()  > rhand);	break;
		case OpCode::gte:   	rhand = pop(); push(pop() >= rhand);  	break;
		case OpCode::neq:   	rhand = pop(); push(pop() != rhand);	break;
		case OpCode::lor:   	rhand = pop(); push(pop() || rhand);	break;
		case OpCode::land:  	rhand = pop(); push(pop() && rhand);	break;
		case OpCode::pushConst: push(ir.addr);							break;
		case OpCode::pushVar:	push(base(ir.level) + ir.addr);			break;
		case OpCode::eval: 		{   auto ea = pop(); push(stack[ea]); } break;
		case OpCode::assign:
			lastWrite = pop();				// Save the effective address for dump()...
			stack[lastWrite] = pop();
			break;
		case OpCode::call: 		call(ir.level, ir.addr);				break;
		case OpCode::ret:   	ret();  								break;
		case OpCode::reti: 		reti();									break;
		case OpCode::enter: 	mkStackSpace(ir.addr);	sp += ir.addr;  break;
		case OpCode::jump:		pc = ir.addr;							break;
		case OpCode::jneq:		if (pop() == 0) pc = ir.addr;			break;
		case OpCode::halt:		return Result::halted;					break;

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

			} else if (sp >= static_cast<Integer> (stack.size())) {
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
}

