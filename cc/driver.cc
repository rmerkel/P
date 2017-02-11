/** @file driver.cc
 *
 * PL0 compiler/interpater driver.
 */

#include "pl0com.h"
#include "pl0int.h"

#include <iostream>

using namespace std;
using namespace pl0;

/** pl0 compiler and interpreter
 *
 *	@return The number of compilier/interpreter errors.
 */
int main(int argc, char* argv[]) {
	const	string			progName = argv[0];
			bool 			verbose = false;
			PL0Compilier	comp {progName};
			InstrVector		program;

	for (int argn = 1; argn < argc; ++argn) {
		const string arg = argv[argn];

		if (arg == "-?" || arg == "-help") {
			cerr << "Usage: " << progName << ": [options]\n"
				 << "Where options is zero or more of the following:\n"
				 << "-?        Print this message and exit.\n"
				 << "-help     Same as -?\n"
				 << "-verbose  Set verbose mode.\n"
				 << "-v        Same as -v\n";
			return EXIT_FAILURE;

		} else if (arg == "-v" || arg == "-verbose")
			verbose = true;

		else {
			cerr << progName << ": unknown option: '" << arg << "'\n";
			return EXIT_FAILURE;
		}
	}

	unsigned nErrors = comp(program, verbose);
	if (0 == nErrors) {
		PL0Interp machine;						// Create the machine and load a test program

		if (verbose) cout << progName << ": loading program from standard input, and starting pl/0...\n";
		const size_t n = machine(program, verbose);
		if (verbose) cout << progName << ": Ending pl/0 after " << n << " machine cycles\n";
	}
}

