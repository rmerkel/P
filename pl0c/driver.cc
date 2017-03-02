/** @file driver.cc
 * 
 * @mainpage PL/0C Programming Language
 *  
 * A compiler/interpreter, inspired by the original PL/0 language and machine from Algorithms + Data
 * Structures = Programs, 1st Edition, by Wirth. Modified to use C/C++ operators, e.g., check for 
 * equality with "==" and assigned with "=", but otherwise using Pascal "lite" syntax and program 
 * nesting. The result is a dialect that I call "PL/0C". 
 *  
 * Like PL/0, PL/0C is a combination compilier and interpreter. If the compiler (pl0c::Comp) 
 * doesn't report any errors, then the generated code is run in the interpreter (pl0c::Interp). 
 *  
 * By default, pl0c::Comp writes a listing to standard output, but the verbose (-v) option will 
 * also log tokens found and code emitted. 
 *  
 * By default, pl0c::Interp logs the writes ("pop's") on standard output, but the verbose (-v) 
 * option will single step the program, writting a state report as well. 
 *  
 * @section	driver-bugs	Bugs 
 *  
 * - Currently, the only way to read from standard input is by way of the "-" file. It would be 
 *   nice if standard input was the default.
 */

#include "pl0ccomp.h"
#include "pl0cinterp.h"

#include <iostream>
#include <vector>

using namespace std;

static string	progName;						///< This programs name
static string	inputFile {"-"};				///< Source file name, or - for standard input
static bool		verbose = false;				///< Verbose messages if true

/// Print a usage message on standard error output
static void help() {
	cerr << "Usage: " << progName << ": [options[ filename\n"
		 << "Where options is zero or more of the following:\n"
		 << "-?        Print this message and exit.\n"
		 << "-help     Same as -?\n"
		 << "-verbose  Set verbose mode.\n"
		 << "-v        Same as -verbose.\n"
		 << "\n"
		 << "And where filename is the name of the source file, or '-' for standard input.\n";
}

/** Parse the command line arguments...
 *
 * @return false if an command line syntax error is encounter, or help requested.
 */
static bool parseCommandline(const vector<string>& args) {
	for (auto arg : args) {
		if (arg.empty()) 
			continue;							// skip ""
												
		else if ("-" == arg) 
			inputFile = arg;					// read from standard input

		else if ("-help" == arg) {
			help();
			return false;

		} else if ("-verbose" == arg) 
			verbose = true;						// annoy the user with lots-o-messages...

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
			inputFile = arg;				// Read from named file
	}

	return true;
}

/** PL/0C compiler and interpreter
 *  
 * Usage: PL0C [options] [file] 
 *  
 * Compiles, and if there are no errors, runs the input program. 
 *  
 * @return The number of compiler/interpreter errors.
 */
int main(int argc, char* argv[]) {
	progName = argv[0];

	pl0c::Comp			comp{progName};			// The compiler...
	pl0c::Interp 		machine;				// The machine...
	pl0c::InstrVector	code;					// Machine instructions...
	unsigned 			nErrors = 0;

	vector<string> args;						// Parse the command line arguments...
	for (int argn = 1; argn < argc; ++argn)
		args.push_back(argv[argn]);

	if (!parseCommandline(args))
		++nErrors;

	if (inputFile.empty()) {
		cerr << progName << ": source file name was not found!\n";
		++nErrors;
												// Compile the source, run if no errors
	} else if (0 == (nErrors = comp(inputFile, code, verbose))) {
		if (verbose) cout << progName << ": loading program from standard input, and starting pl/0...\n";
		const size_t n = machine(code, verbose);
		if (verbose) cout << progName << ": Ending pl/0 after " << n << " machine cycles\n";
	}

	return nErrors;
}
