/** @file pl0int.cpp
 *
 *	PL/0 interpreter in C++
 *
 *	Ported from p0com.p, from Algorithms + Data Structions = Programes. Changes include
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

#include "pl0int.h"

using namespace std;

// private:

/// Dissamble the current instruction (ir)
void pl0::disasm() {

	const unsigned level = ir.level;			// so we don't diplay as a charactor
	const Instr ir = code[pc];

	cout << setw(4+5) << pc << ": ";
    switch(ir.op) {
    case Const:	cout << "Const "	<< ir.addr;						break;
    case Push:	cout << "Push "		<< level << ", " << ir.addr;	break;
    case Pop:	cout << "Pop "		<< level << ", " << ir.addr;	break;
    case Call:	cout << "Call "		<< level << ", " << ir.addr;	break;
    case Enter:	cout << "Enter "	<< ir.addr;						break;
    case Jump:	cout << "Jump "		<< ir.addr;						break;
    case JNE:	cout << "JNE "		<< ir.addr;						break;
    case Ret:	cout << "Ret";										break;	
	case Neg:	cout << "Neg";										break;
    case Add:	cout << "Add";										break;
    case Sub:	cout << "Sub";										break;
    case Mul:	cout << "Mul";										break;
    case Div:	cout << "Div";										break;
	case Odd:	cout << "Odd";										break;
    case Equ:	cout << "Equ";										break;
    case Neq:	cout << "Neq";										break;
    case LT:	cout << "LT";										break;
    case LTE:	cout << "LTE";										break;
    case GT:	cout << "GT";										break;
    case GTE:	cout << "GTE";										break;
	default:
		cerr << "Unknown op code: " << static_cast<unsigned>(ir.op) << level << ", " << ir.addr << endl;
		assert(false);
	}
	cout << "\n";
}

/// Dump the current machine state
void pl0::dump() {
	// Dump the last write
	if (lastWrite.valid())
		cout << "    " << setw(5) << lastWrite << ": " << stack[lastWrite] << endl;
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

	disasm();

	cout << endl;
}

// protected:

/// Find the base 'lvl' levels (frames) up the stack
uint16_t pl0::base(uint16_t lvl) {
	uint16_t b = bp;
	for (; lvl > 0; --lvl)
		b = stack[b];

	return b;
 }

/// Run the machine from it's current state
size_t pl0::run() {
	size_t cycles = 0;							// # of instrucitons run to date

	if (verbose)
		cout << "Reg  Addr Value/Instr\n"
			 << "---------------------\n";

  	do {
		assert(pc < code.size());
		// we assume that the stack size is less than numberic_limits<int>::max()!
		if (-1 != sp) assert(sp < static_cast<int>(stack.size()));


		dump();									// Dump state and disasm the next instruction
		ir = code[pc++];						// Fetch next instruction...
		++cycles;

    	switch(ir.op) {
        case Const:	stack[++sp] = ir.addr;							break;
    	case Ret: 							// return
    			sp = bp - 1; pc = stack[sp + 3]; bp = stack[sp + 2];
    			break;

		case Neg:	stack[sp] = -stack[sp];							break;
    	case Add:	--sp; stack[sp] = stack[sp] + stack[sp+1];		break;
    	case Sub:	--sp; stack[sp] = stack[sp] - stack[sp+1];		break;
    	case Mul:	--sp; stack[sp] = stack[sp] * stack[sp+1];		break;
    	case Div:	--sp; stack[sp] = stack[sp] / stack[sp+1];		break;
		case Odd:	stack[sp] = stack[sp] & 1;						break;
    	case Equ:	--sp; stack[sp] = stack[sp] == stack[sp+1];		break;
    	case Neq:	--sp; stack[sp] = stack[sp] != stack[sp+1];		break;
    	case LT:	--sp; stack[sp] = stack[sp]  < stack[sp+1];		break;
    	case GTE:	--sp; stack[sp] = stack[sp] >= stack[sp+1];		break;
    	case GT:	--sp; stack[sp] = stack[sp]  > stack[sp+1];		break;
    	case LTE:	--sp; stack[sp] = stack[sp] <= stack[sp+1];		break;
        case Push:	stack[++sp] = stack[base(ir.level) + ir.addr];	break;
        case Pop:
			lastWrite = base(ir.level) + ir.addr;// Save the effective address for dump()
			stack[lastWrite] = stack[sp--];
			break;

        case Call: 								// Call a procedure
			// Create a enw frame/mark block that the bp register points to
        	stack[sp+1] = base(ir.level);
			stack[sp+2] = bp;
			stack[sp+3] = pc;
        	bp = sp + 1;
			pc = ir.addr;
			break;

        case Enter:	sp += ir.addr;									break;
        case Jump:	pc = ir.addr;									break;
        case JNE:	if (stack[sp--] == 0) pc = ir.addr;				break;
		default:
			assert(false);
		};

	} while (pc != 0);

	return cycles;
}

//public

/** Default constructor; results in an idle, reset, machine
 *
 *	@param 	ver		True for verbose debugging messages
 *  @param	stacksz	Maximum depth of the data segment/stack, in machine Words
 */
pl0::pl0(bool ver, size_t stacksz) : stack(stacksz), verbose{ver} {
	reset();
}

/// Load a test applicaton and start the pl/0 machine running...
size_t pl0::operator()(const Instr* begin, const Instr* end) {
	// Fill the stack with -1s for debugging...
	fill(stack.begin(), stack.end(), -1);

	copy(begin, end, back_inserter(code));
	reset();
	return run();
}

/// Reset the machine back to it's initial state.
void pl0::reset() {
	pc = 0, bp = 0, sp = -1;	// Set up the initial mark block/frame...
	stack[0] = stack[1] = stack[2] = 0;
}

#if 0
static string progName = "pl0int";

/// Load a test applicaton and start the pl/0 machine running...
int main(int argc, char** argv) {
	bool verbose = false;							// Don't output verbose messages

	progName = argv[0];
	for (int argn = 1; argn < argc; ++argn) {		// Parse argv[1,argc) for options...
		const string arg = argv[argn];

		auto i = arg.begin();
		if (*i++ != '-') {
			cerr << progName << ": unknown parameter: " << arg << endl;
			return EXIT_FAILURE;

		} else for (; i < arg.end(); ++i) {
			switch(*i) {
			case '?':
				cerr	<< "Usage: " << progName << " [options]\n"
						<< "Where options is one or more of the following:\n"
						<< "-?    - print this message and exit\n"
						<< "-help - print this message and exit\n"
						<< "-v    - verbose messages\n";
				return EXIT_FAILURE;;
				break;
			case 'v':	verbose = true;	break;
			default:
				cerr << progName << ": unknown parameter: " << arg << endl;
				return EXIT_FAILURE;
			}
		}
	}

	pl0 machine(verbose);							// Create the machine and load a test program
	pl0::Instr fact[] = {							// Factorial program
    	{	pl0::Jump,  0, 19	},					//  0: jump to main
    	{	pl0::Jump,  0,  2	},					//  1: jump to factorial

		/*
		 *	var p, n;
	     *	procedure factorial;
	     *	begin
	     *		p := 1;
	     *		while n > 1 do begin
	     *			p := p * n;
	     *			n := n - 1;
	     *		end;
	     *	end;
		 */

    	{	pl0::Enter,  0,	 3	},					//  2: factorial
		{	pl0::Const,  0,	 1	},
		{	pl0::Pop,  1,	 3	},					//  4: p := 1
		{	pl0::Push,  1,	 4	},					//  5: while...
		{	pl0::Const,  0,	 1	},
		{	pl0::GT				},					//  7: n > 1
		{	pl0::JNE,  0,	18	},
		{	pl0::Push,  1,	 3	},
		{	pl0::Push,  1,	 4	},
		{	pl0::Mul			},
		{	pl0::Pop,  1,	 3	},					// 12: p := p * n
		{	pl0::Push,  1,	 4	},
		{	pl0::Const,  0,	 1	},
		{	pl0::Sub			},
		{	pl0::Pop,  1,	 4	},					// 16: n := n - 1;
		{	pl0::Jump,  0,	 5	},
		{	pl0::Ret			},					// 18: ret
 
	   /*
	    *	block 0
		*		p = [base + 3]
		*		n = [base + 4]
		*
	    *	begin
	    *		n := 15;
	    *		call factorial;
	    *	end.
		*/

		{	pl0::Enter,  0,	 5	},					// 19: main
		{	pl0::Const,  0,	15	},
		{	pl0::Pop,  0,	 4	},					// 22: n := 15;
		{	pl0::Call,  0,	 2	},
		{	pl0::Ret			}
	};
	const pl0::Instr* efact = fact + sizeof(fact) / sizeof(pl0::Instr);

	if (verbose) cout << "loading Factorial program and starting pl/0...\n";
	const size_t n = machine(fact, efact);
	if (verbose) cout << "Ending pl/0 after " << n << " machine cycles\n";

	return EXIT_SUCCESS;
}
#endif
