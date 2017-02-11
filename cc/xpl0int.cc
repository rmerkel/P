/** @file xpl0int.cc
 *
 *	Test the PL/0 interpreter in C++
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
using namespace pl0;

/// Load a test applicaton and start the pl/0 machine running...
int main(int argc, char** argv) {
	bool verbose = false;							// Don't output verbose messages

	const string progName = argv[0];
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

	PL0Interp machine;							// Create the machine and load a test program
	InstrVector fact = {						// Factorial program
    	{	OpCode::Jump,		0, 19	},		//  0: jump to main
    	{	OpCode::Jump,		0,  2	},		//  1: jump to factorial

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

    	{	OpCode::Enter,		0,	 3	},		//  2: factorial
		{	OpCode::pushConst,	0,	 1	},
		{	OpCode::Pop,  		1,	 3	},		//  4: p := 1
		{	OpCode::pushVar,	1,	 4	},		//  5: while...
		{	OpCode::pushConst,	0,	 1	},
		{	OpCode::GT					},		//  7: n > 1
		{	OpCode::Jne,  		0,	18	},
		{	OpCode::pushVar,	1,	 3	},
		{	OpCode::pushVar,	1,	 4	},
		{	OpCode::Mul					},
		{	OpCode::Pop, 		1,	 3	},		// 12: p := p * n
		{	OpCode::pushVar,	1,	 4	},
		{	OpCode::pushConst,	0,	 1	},
		{	OpCode::Sub					},
		{	OpCode::Pop,  		1,	 4	},		// 16: n := n - 1;
		{	OpCode::Jump,  		0,	 5	},
		{	OpCode::Return				},		// 18: ret
 
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

		{	OpCode::Enter,		0,	 5	},		// 19: main
		{	OpCode::pushConst,	0,	15	},
		{	OpCode::Pop,  		0,	 4	},		// 22: n := 15;
		{	OpCode::Call,  		0,	 2	},
		{	OpCode::Return				}
	};

	if (verbose) cout << progName << ": loading Factorial program and starting pl/0...\n";
	const size_t n = machine(fact, verbose);
	if (verbose) cout << progName << ": ending pl/0 after " << n << " machine cycles\n";

	return EXIT_SUCCESS;
}

