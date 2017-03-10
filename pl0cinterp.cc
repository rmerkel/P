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

	/**
	 * Unlinks the stack frame, setting the return address as the next instruciton.
	 */
	void Interp::ret() {
		sp = fp - 1; 					// "pop" the activaction frame
		pc = stack[fp + FrameRetAddr];
		fp = stack[fp + FrameOldBp];
		sp -= ir.addr;					// Pop parameters, if any...
	}

	/**
	 *  @return	Result::success, or ...
	 */
	Interp::Result Interp::run() {
		if (verbose)
			cout << "Reg  Addr Value/Instr\n"
				 << "---------------------\n";

		do {
			if (pc >= code.size()) {
				cerr << "pc (" << pc << ") is out of range: [0.." << code.size() << ")!\n";
				return Result::badFetch;
			}

			// we assume that the stack size is less than numberic_limits<int>::max()!
			assert(sp < static_cast<int>(stack.size()));


			dump();								// Dump state and disasm the next instruction
			ir = code[pc++];					// Fetch next instruction...
			++ncycles;

			switch(ir.op) {

			// unary operations

			case OpCode::Not:		stack[sp] = !stack[sp];						break;
			case OpCode::neg:		stack[sp] = -stack[sp];						break;
			case OpCode::comp:		stack[sp] = ~stack[sp];						break;

			// binrary operations

			case OpCode::add:		--sp; stack[sp] = stack[sp] + stack[sp+1];	break;
			case OpCode::sub:		--sp; stack[sp] = stack[sp] - stack[sp+1];	break;
			case OpCode::mul:		--sp; stack[sp] = stack[sp] * stack[sp+1];	break;
			case OpCode::div:
				--sp;
				if (stack[sp+1] == 0) {
					--pc; 			// backup
					cerr << "attempt to divide by zero @ pc (" << pc << ")!\n";
					return Result::divideByZero;
				}
				stack[sp] = stack[sp] / stack[sp+1];
				break;

			case OpCode::rem: 		--sp; stack[sp] = stack[sp] % stack[sp+1];	break;

			case OpCode::bor:		--sp; stack[sp] = stack[sp] | stack[sp+1];	break;
			case OpCode::band:		--sp; stack[sp] = stack[sp] && stack[sp+1];	break;
			case OpCode::bxor:		--sp; stack[sp] = stack[sp] ^ stack[sp+1];	break;

			case OpCode::lshift:	--sp; stack[sp] = stack[sp] << stack[sp+1];	break;
			case OpCode::rshift:	--sp; stack[sp] = stack[sp] >> stack[sp+1];	break;

			case OpCode::equ:		--sp; stack[sp] = stack[sp] == stack[sp+1];	break;
			case OpCode::neq:		--sp; stack[sp] = stack[sp] != stack[sp+1];	break;
			case OpCode::lt:		--sp; stack[sp] = stack[sp]  < stack[sp+1];	break;
			case OpCode::gte:		--sp; stack[sp] = stack[sp] >= stack[sp+1];	break;
			case OpCode::gt:		--sp; stack[sp] = stack[sp]  > stack[sp+1];	break;
			case OpCode::lte:		--sp; stack[sp] = stack[sp] <= stack[sp+1];	break;
			case OpCode::lor:		--sp; stack[sp] = stack[sp] || stack[sp+1];	break;
			case OpCode::land:		--sp; stack[sp] = stack[sp] && stack[sp+1];	break;

			// push/pop..

			case OpCode::pushConst:
				mkStackSpace(1);
				stack[++sp] = ir.addr;
				break;

			case OpCode::pushVar:
				mkStackSpace(1);
				stack[++sp] = base(ir.level) + ir.addr;
				break;

			case OpCode::eval: {	
					auto ea = stack[sp];
					stack[sp] = stack[ea];
				}
				break;

			case OpCode::assign:
				lastWrite = stack[sp--];		// Save the effective address for dump()
				stack[lastWrite] = stack[sp--];
				break;

			// control flow...

			case OpCode::call: 					// Call a subroutine
				mkStackSpace(FrameSize);

				// Push a new activation frame block on the stack
				stack[sp + 1 + FrameBase] 		= base(ir.level);
				stack[sp + 1 + FrameOldBp] 		= fp;
				stack[sp + 1 + FrameRetAddr]	= pc;
				stack[sp + 1 + FrameRetVal] 	= 0;
				fp = sp + 1;					// Points to start of the frame
				sp += FrameSize;				// Points to the return value...
				pc = ir.addr;					// Set the subroutine'saddress
				break;

			case OpCode::ret: 					// Return from a procedure
				ret();
				break;

			case OpCode::reti: {				// Return integer from function
												// Save the function result...
					auto temp = stack[fp + FrameRetVal];
					ret();						// Unlink the stack frame...
					stack[++sp] = temp;			// Push the result
				}
				break;


			case OpCode::enter:	
				mkStackSpace(ir.addr);
				sp += ir.addr;
				break;

			case OpCode::jump:	pc = ir.addr;								break;
			case OpCode::jneq:	if (stack[sp--] == 0) pc = ir.addr;			break;

			default:
				cerr << "Unknown op code: " << pl0c::toString(ir.op) << endl;
				return Result::unknownInstr;
			};

		} while (pc != 0);
		dump();						// Dump the exit state

		return Result::success;
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
		return run();
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
		case Result::success:		return "success";		break;
		case Result::divideByZero:	return "divideByZero";	break;
		case Result::badFetch:		return "badFetch";		break;
		case Result::unknownInstr:	return "unknownInstr";	break;
		default:					return "undefined error!";
		}
	}
}

