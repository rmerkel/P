/** @file pl0cinterp.cc
 *
 *	PL/0 interpreter in C++
 *
 *	Ported from p0com.p, from Algorithms + Data Structures = Programs. Changes include
 *	- index the stack (stack[0..maxstack-1]); thus the initial values for the t and b registers are
 *    -1 and 0.
 *  - replaced single letter variables, e.g., p is now pc.
 *
 *	Currently loads and runs a sample/test program, dumping machine state before each instruction
 *	fetch.
 */

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "pl0cinterp.h"

using namespace std;
using namespace pl0c;

// private:

/// Dump the current machine state
void PL0CInterp::dump() {
	// Dump the last write
	if (lastWrite.valid())
		cout << "    " << setw(5) << lastWrite << ": " << stack[lastWrite] << std::endl;
	lastWrite.invalidate();

	if (!verbose) return;

	// Dump the current mark block (frame)...
	if (sp >= bp) {						// Block/frame established?
		cout <<		"bp: " << setw(5) << bp << ": " << stack[bp] << endl;
		for (int bl = bp+1; bl < sp; ++bl)
			cout <<	"    " << setw(5) << bl << ": " << stack[bl] << endl;

		if (sp != -1)
			cout <<	"sp: " << setw(5) << sp << ": " << stack[sp] << endl;
		else
			cout <<	"sp: " << setw(5) << sp << endl;

	} else {							// Procedure hasn't called 'int' yet
		if (sp != -1)
			cout <<	"sp: " << setw(5) << sp << ": " << stack[sp] << endl;

		else
			cout <<	"sp: " << setw(5) << sp << endl;
		cout <<		"bp: " << setw(5) << bp << ": " << stack[bp] << endl;
	}

	disasm(pc, code[pc], "pc");

	cout << endl;
}

// protected:

/// Find the base 'lvl' levels (frames) up the stack
uint16_t PL0CInterp::base(uint16_t lvl) {
	uint16_t b = bp;
	for (; lvl > 0; --lvl)
		b = stack[b];

	return b;
 }

/** Run the machine from it's current state
 *  @return	the number of machine cycles run
 */
size_t PL0CInterp::run() {
	size_t cycles = 0;						// # of instructions run to date

	if (verbose)
		cout << "Reg  Addr Value/Instr\n"
			 << "---------------------\n";

  	do {
		assert(pc < code.size());
		// we assume that the stack size is less than numberic_limits<int>::max()!
		if (-1 != sp) assert(sp < static_cast<int>(stack.size()));


		dump();								// Dump state and disasm the next instruction
		ir = code[pc++];					// Fetch next instruction...
		++cycles;

    	switch(ir.op) {

 		// unary operations

		case OpCode::neg:	stack[sp] = -stack[sp];						break;
		case OpCode::odd:	stack[sp] = stack[sp] & 1;					break;

		// binrary operations

    	case OpCode::add:	--sp; stack[sp] = stack[sp] + stack[sp+1];	break;
    	case OpCode::sub:	--sp; stack[sp] = stack[sp] - stack[sp+1];	break;
    	case OpCode::mul:	--sp; stack[sp] = stack[sp] * stack[sp+1];	break;
    	case OpCode::div:	--sp; stack[sp] = stack[sp] / stack[sp+1];	break;
    	case OpCode::equ:	--sp; stack[sp] = stack[sp] == stack[sp+1];	break;
    	case OpCode::neq:	--sp; stack[sp] = stack[sp] != stack[sp+1];	break;
    	case OpCode::lt:	--sp; stack[sp] = stack[sp]  < stack[sp+1];	break;
    	case OpCode::gte:	--sp; stack[sp] = stack[sp] >= stack[sp+1];	break;
    	case OpCode::gt:	--sp; stack[sp] = stack[sp]  > stack[sp+1];	break;
		case OpCode::lte:	--sp; stack[sp] = stack[sp] <= stack[sp+1];	break;

		// push/pop

		case OpCode::pushConst:
			stack[++sp] = ir.addr;
			break;

        case OpCode::pushVar:
        	stack[++sp] = stack[base(ir.level) + ir.addr];
        	break;

        case OpCode::pop:
        	// Save the effective address for dump()
			lastWrite = base(ir.level) + ir.addr;
			stack[lastWrite] = stack[sp--];
			break;

        case OpCode::call: 					// Call a procedure
			// Create a new frame/mark block on the stack
        	stack[sp+1] = base(ir.level);	// Sent previous frame base
			stack[sp+2] = bp;				// Save bp register
			stack[sp+3] = pc;				// Save return address
        	bp = sp + 1;					// points to the frame base
			pc = ir.addr;					// Set the procedure address
			break;

		case OpCode::ret: 					// Return from a procedure
			sp = bp - 1; 					// Restore the previous sp
			pc = stack[sp + 3]; 			// Restore the return address
			bp = stack[sp + 2];				// Restore the previous bp
			sp -= ir.addr;					// Pop parameters
			break;

		case OpCode::enter:	sp += ir.addr;								break;
        case OpCode::jump:	pc = ir.addr;								break;
        case OpCode::jneq:	if (stack[sp--] == 0) pc = ir.addr;			break;
		default:
			cerr << "Unknown op code: " << toString(ir.op) << endl;
			assert(false);
		};

	} while (pc != 0);
	dump();						// Dump the exit state

	return cycles;
}

//public

/** Default constructor
 *
 *  @param	stacksz	Maximum depth of the data segment/stack, in machine Words
 */
PL0CInterp::PL0CInterp(size_t stacksz) : stack(stacksz), verbose{false} {
	reset();
}

/** Load a applicaton and start the pl/0 machine running...
 *
 *	@param	program	The program to load and run
 *	@param 	ver		True for verbose debugging messages
 *  @return	The number of machine cycles run
 */
size_t PL0CInterp::operator()(const InstrVector& program, bool ver) {
	verbose = ver;

	// Fill the stack with -1s for debugging...
	fill(stack.begin(), stack.end(), -1);

	code = program;
	reset();
	return run();
}

/// Reset the machine back to it's initial state.
void PL0CInterp::reset() {
	pc = 0, bp = 0, sp = -1;	// Set up the initial mark block/frame...
	stack[0] = stack[1] = stack[2] = 0;
}

