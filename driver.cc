/** @file driver.cc
 *
 * The driver routine for the PL/0C Programming Language compiler/interpreter.
 *
 * @mainpage PL/0C Programming Language
 *
 * Yet another small, interpreted, computer language... just for the fun of it!
 *
 * A compiler/interpreter, inspired by the original PL/0 language and machine described in
 * *Algorithms + Data Structures = Programs*, 1st Edition, by Wirth. Modified to use C/C++
 * operators, e.g., check for equality with "==" and assigned with "=", but otherwise using Pascal
 * "light" syntax and program nesting. The result is a dialect that I call "PL/0C".
 *
 * Like PL/0, PL/0C is a combination compiler and interpreter; it first runs the compiler
 * (pl0c::Comp), and if no errors where encountered, it runs the results in the interpreter
 * (pl0c::Interp). A listing and machine output are written to standard output.
 *
 * The compiler started life as a copy of the C example at
 * https://en.wikipedia.org/wiki/Recursive_descent_parser, modified to emit code per Wirth's
 * interpret procedure while using the TokenStream class (pl0c::TokenStream) from *The C++
 * Programming Language*, 4th Edition, by Stroustrup. Finally modified to support the PL/0C
 * dialect. By default, pl0c::Comp writes a listing to standard output, but the verbose (-v) option
 * will also log tokens found and code emitted.
 *
 * The machine/interpreter stated life as a C/C++ port of Wirth's machine (procedure interpret),
 * modified to use lest "weird" instruction names, modified to use indirect addressing per HOC, as
 * described in *The UNIX Programming Environment*, by Kernighan and Pike. By default, pl0c::Interp
 * logs "assigns" on standard output, but the verbose (-v) option will run the program in trace
 * mode; disassembling and dumping the activation frame for each instruction single step the
 * program.
 *
 * @bug
 * - No arrays or strings, just signed integers
 * - No constant statements, i.e., constants must be initialized with a simple number.
 * - No input/output instructions.
 * - No interactive mode for debugging; just automatic single stepping (verbose == true)
 *
 * @version 1.0 - Initial release
 * @version 1.1
 *  - Added Pascal style comments
 *  - TokenString now returns unknown for an unknow token, or badComment for an unterminted
 *    comment.
 *  - Folded factors into expressions
 *  - Added logical and, orr plus bitwise and, or, xor.
 * @version 1.2 - Added halt instruction
 * @version 1.3
 *	- Interp registers are now all vector<T>size_type
 *  - Added xpl0c.sh to run regression tests
 *  - Added Datum, the stack data type, as a step towards adding types to pl0c, and for unsigned
 *    binary operations.
 *  - Parser now understands, but ignores type names for variables, but not functions.
 *
 * @version 1.3a
 *  - Adding types "integer" and real...
 *  - Adding arrayws...
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "pl0ccomp.h"
#include "pl0cinterp.h"

#include <iostream>
#include <vector>

using namespace std;

static	string	progName;						///< This programs name
static 	string	inputFile {"-"};				///< Source file name, or - for standard input
static 	bool	verbose = false;				///< Verbose messages if true

/// Print a usage message on standard error output
static void help() {
	cerr << "Usage: " << progName << ": [options[ [filename]\n"
		 << "Where options is zero or more of the following:\n"
		 << "-?        Print this message and exit.\n"
		 << "-help     Same as -?\n"
		 << "-verbose  Set verbose mode.\n"
		 << "-v        Same as -verbose.\n"
 		 << "-version  Print the program version.\n"
		 << "-V        Same as -version.\n"
		 << "\n"
		 << "filename  The name of the source file, or '-' or '' for standard input.\n";
}

/// Print the version number as major.minor
static void printVersion() {
	cout << progName << ": verson: 1.3b\n";	// makesure to update the verison in mainpage!!
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

		else if ("-version" == arg)
			printVersion();

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

				case 'V':
					printVersion();
					break;

				default:
					cerr << progName << ": unknown command line parameter: -" << arg[n] << "\n";
					return false;
				}

		} else
			inputFile = arg;				// Read from named file
	}

	if (inputFile.empty())
		inputFile = "-";					// Default to standard input
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
												// Compile the source, run if no errors
	else if (0 == (nErrors = comp(inputFile, code, verbose))) {
		if (verbose) {
			if (inputFile == "-")
				cout << progName << ": loading program from standard input, and starting pl/0c...\n";
			else
				cout << progName << ": loading program '" << inputFile << "', and starting pl/0c...\n";
		}

		const pl0c::Interp::Result r = machine(code, verbose);
		if (pl0c::Interp::Result::success != r)
			cerr << progName << ": runtime error: " << pl0c::Interp::toString(r) << "!\n";

		if (verbose) cout << progName << ": Ending pl/0c after " << machine.cycles() << " machine cycles\n";
	}

	return nErrors;
}
