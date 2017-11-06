/** @file driver.cc
 *
 * @tableofcontents
 *
 * The driver routine for the Pascal-lite Programming Language compiler/interpreter.
 *
 * @mainpage Pascal-lite Programming Language
 *
 * Yet another small, interpreted, computer language... just for the fun of it!
 *
 * A compiler/interpreter, inspired by the original PL/0 language and machine described in
 * Algorithms + Data Structures = Programs*, 1st Edition, by Wirth.  Currently, the
 * compilier supports a dialect between PL/0 and Pascal.
 *
 * Like PL/0, 'pas' is a combination compiler and interpreter; it first runs the compiler
 * (PasComp), and if no errors where encountered, it runs the results in the interpreter
 * (Interp). A listing and machine output are written to standard output.
 *
 * The compiler started life as a copy of the C example at
 * https://en.wikipedia.org/wiki/Recursive_descent_parser, modified to emit code per Wirth's
 * interpret procedure while using the TokenStream class (TokenStream) from *The C++
 * Programming Language*, 4th Edition, by Stroustrup. Finally, modified, step by step to 
 * support Pascal proper. By default, PasComp writes a listing to standard output, but the
 *  verbose (-v) option will also log tokens found and code emitted.
 *
 * The machine/interpreter stated life as a C/C++ port of Wirth's machine (procedure interpret),
 * modified to use lest "weird" instruction names, modified to use indirect addressing per HOC, as
 * described in *The UNIX Programming Environment*, by Kernighan and Pike. By default, Interp
 * logs "assigns" on standard output, but the verbose (-v) option will run the program in trace
 * mode; disassembling and dumping the activation frame for each instruction single step the
 * program.
 *
 * @bug
 * - More Pascal than pl0c at this point
 * - No files, characters, strings, just signed integers and reals
 * - No input instructions, and just elementary Write/Writeln implementation.
 * - No interactive mode for debugging; just automatic single stepping (-v)
 * - No pointers or dynamic memory
 *
 * @version 0.1  - Initial fork from pl0c, using pascal <>, =, etc...
 * @version 0.2  - Program declaraction
 * @version 0.3  - Removed the C/C++ bit and shift operations
 * @version 0.4  - Added types, limited to ranges and arrays.
 * @version 0.5  - Added enumerations
 * @version 0.6  - Fixed array index type checks
 * @version 0.7  - Supports non-zero based array indexes
 * @version 0.8  - Supports multiple dimensioned arrays. Fixed promotion bug
 * @version 0.9  - Supports a[i,j] and a[i][j] syntax
 * @version 0.10 - Adding built-in functions and constants, refactored types, capitalized
 * 				   built-in "nouns" and functions. WIP.
 * @version 0.11 - Primitive Write and Writeln, refactoring.
 * @version 0.12 - Limit checks, successor, predecessor. for statement.
 * @version 0.13 - Refactored TDesc into iTDesc and it's derived classes
 * @version 0.14 - Write/Writeln field specifiers.
 * @version 0.15 - Records and selectors
 * @version 0.16 - Refactored records, selectors and arrays. Added New(), Dispose() and nil
 *  
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "pascomp.h"
#include "interp.h"

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
	cout << progName << ": verson: 0.16\n";		// make sure to update the verison in mainpage!!
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

/** Pascal compiler and interpreter
 *
 * Usage: pas [options] [file]
 *
 * Compiles, and if there are no errors, runs the input program.
 *
 * @return The number of compiler/interpreter errors.
 */
int main(int argc, char* argv[]) {
	progName = argv[0];

	PasComp		comp{progName};					// The compiler...
	Interp 		machine;						// The machine...
	InstrVector	code;							// Machine instructions...
	unsigned 	nErrors = 0;

	vector<string> args;						// Parse the command line arguments...
	for (int argn = 1; argn < argc; ++argn)
		args.push_back(argv[argn]);

	if (!parseCommandline(args))
		++nErrors;
												// Compile the source, run if no errors
	else if (0 == (nErrors = comp(inputFile, code, verbose))) {
		if (verbose) {
			if (inputFile == "-")
				cout << progName << ": loading program from standard input, and starting pascal-lite...\n";
			else
				cout << progName << ": loading program '" << inputFile << "', and starting pascal-lite...\n";
		}

		const Interp::Result r = machine(code, verbose);
		if (Interp::success != r)
			cerr << progName << ": runtime error: " << r << "!\n";

		if (verbose) cout << progName << ": Ending pascal-lite after " << machine.cycles() << " machine cycles\n";
	}

	return nErrors;
}

