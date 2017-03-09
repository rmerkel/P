/** @file driver.cc
 *  
 * The driver routine for the PL/0C Programming Langauge compilier/interpreter. 
 *  
 * @mainpage PL/0C Programming Language
 *  
 * A compiler/interpreter, inspired by the original PL/0 language and machine described in
 * Algorithms + Data Structures = Programs, 1st Edition, by Wirth. Modified to use C/C++ operators, 
 * e.g., check for equality with "==" and assigned with "=", but otherwise using Pascal "lite" 
 * syntax and program nesting. The result is a dialect that I call "PL/0C". 
 *  
 * Like PL/0, PL/0C is a combination compilier and interpreter; it first runs the compiler 
 * (pl0c::Comp), and if no errors where encounted, it runs the reulsts in the interpreter 
 * (pl0c::Interp). A listing and machine output are written to standard output. 
 *  
 * The compilier started life as a copy of C example at
 * https://en.wikipedia.org/wiki/Recursive_descent_parser, modified to emit code per Wirth's 
 * interpret procedure while using the TokeStream from The C++ Programming Language, 4th 
 * Edition, by Stroustrup. Finally modified to support the PL/0C dialect. By default, pl0c::Comp 
 * writes a listing to standard output, but the verbose (-v) option will also log tokens found and 
 * code emitted. 
 *  
 * The machine/interpreter stated life as a C/C++ port of With's machine (procedure interpet), 
 * modified to use lest "weird" instruction names, modified to use indirect addressing per HOC, as
 * described in The UNIX Programming Environment, by Kernighan and Pike. By default, pl0c::Interp 
 * logs "assigns" on standard output, but the verbose (-v) option will single step the program, 
 * writing a state report as well. 
 * 
 * @version 1.0f -	Added dynamicly sized stack; check that pc in in range; divide by zero check;
 *  				Interp returns success or error indicator; renamed bp to fp. Cleaned up Token.
 * @author Randy Merkel, Slowly but Surly Software. 
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#include "pl0ccomp.h"
#include "pl0cinterp.h"

#include <iostream>
#include <vector>

using namespace std;

static 			string	progName;				///< This programs name
static 			string	inputFile {"-"};		///< Source file name, or - for standard input
static 			bool	verbose = false;		///< Verbose messages if true

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
	cout << progName << ": verson: 1.0f\n";	// makesure to update the verison in mainpage!!
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
				cout << progName << ": loading program from standard input, and starting pl/0...\n";
			else
				cout << progName << ": loading program '" << inputFile << "', and starting pl/0...\n";
		}

		const pl0c::Interp::Result r = machine(code, verbose);
		if (pl0c::Interp::Result::success != r)
			cerr << progName << ": runtime error: " << pl0c::Interp::toString(r) << "!\n";

		if (verbose) cout << progName << ": Ending pl/0 after " << machine.cycles() << " machine cycles\n";
	}

	return nErrors;
}
