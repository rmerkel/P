/** @file driver.cc
 *
 * PL0 compiler/interpater driver.
 */

#include "pl0com.h"
#include "pl0int.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace pl0;

static string	progName;						///< This programs name
static string	inputFile {"-"};				///< Source file name, or - for standard input
static bool		verbose = false;				///< Verbose messages if true

/// Print a usage message on standard error output
static void help() {
	cerr << "Usage: " << progName << ": [options filename]\n"
		 << "Where options is zero or more of the following:\n"
		 << "-?        Print this message and exit.\n"
		 << "-help     Same as -?\n"
		 << "-verbose  Set verbose mode.\n"
		 << "-v        Same as -v.\n"
		 << "\nAnd where filename is the path/name of the source file, or '-' for standard input.\n";
}

/// Parase the command line arguments...
static bool parseCommandline(const vector<string>& args) {
	for (auto arg : args) {
		if (arg.empty())
			continue;							// Skip ""

		else if ("-" == arg) 
			inputFile = arg;					// Read from standard input

		else if ("-help" == arg) {
			help();
			return false;

		} else if ("-verbose" == arg) 
			verbose = true;

		else if ('-' == arg[0])	{				// parse -options...
			for (unsigned n = 1; n < arg.size(); ++n)
				switch(arg[n]) {
				case '?':
					help();
					return false;
					break;

				case 'v':
					verbose = true;
					break;

				default:
					cerr << progName << ": unknown command line parameter: -" << arg[n] << "\n";
					return false;
				}

		} else
			inputFile = arg;					// Read from named file
	}

	return true;
}


/** pl0 compiler and interpreter
 *  
 * Usage: pl0com [options] [file] 
 *  
 * Compiles, and if there are no errors, runs the input program. 
 *  
 * @return The number of compiler/interpreter errors.
 */
int main(int argc, char* argv[]) {
	progName = argv[0];

	PL0Comp	comp{progName};		// The compiler...
	PL0Interp 		machine;			// The machine...
	InstrVector		code;				// Machine instructions...
	unsigned 		nErrors = 0;

	vector<string> args;				// Parse the command line arguments...
	for (int argn = 1; argn < argc; ++argn)
		args.push_back(argv[argn]);
	if (!parseCommandline(args))
		++nErrors;

										// Compile the source, run if no errors
	else if (0 == (nErrors = comp(inputFile, code, verbose))) {
		if (verbose) cout << progName << ": loading program from standard input, and starting pl/0...\n";
		const size_t n = machine(code, verbose);
		if (verbose) cout << progName << ": Ending pl/0 after " << n << " machine cycles\n";
	}

	return nErrors;
}
