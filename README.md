PL/0C Programming Language

Yet another small, interpreted, computer language... just for the fun of it!

A compiler/interpreter, inspired by the original PL/0 language and machine
described in Algorithms + Data Structures = Programs, 1st Edition, by Wirth.
Modified to use C/C++ operators, e.g., check for equality with "==" and
assigned with "=", but otherwise using Pascal "light" syntax and program
nesting. The result is a dialect that I call "PL/0C".

Like PL/0, PL/0C is a combination compiler and interpreter; it first runs the
compiler (pl0c::Comp), and if no errors where encountered, it runs the results
in the interpreter (pl0c::Interp). A listing and machine output are written to
standard output.

The compiler started life as a copy of the C example at
https://en.wikipedia.org/wiki/Recursive_descent_parser, modified to emit code
per Wirth's interpret procedure while using the TokenStream (pl0c::TokenStream)
from The C++ Programming Language, 4th Edition, by Stroustrup. Finally
modified to support the PL/0C dialect. By default, pl0c::Comp writes a listing
to standard output, but the verbose (-v) option will also log tokens found and
code emitted.

The machine/interpreter stated life as a C/C++ port of Wirth's machine
(procedure interpret), modified to use lest "weird" instruction names, modified
to use indirect addressing per HOC, as described in The UNIX Programming
Environment, by Kernighan and Pike. By default, pl0c::Interp logs "assigns" on
standard output, but the verbose (-v) option will run the program in trace
mode; disassembling and dumping the activation frame for each instruction
single step the program.

Release
 * git add .
 * git commit -m "description..."
 * git tag -a vMajor.Minor -m "summary..."
 * git push --tags origin branch-name

Bugs:
 * No arrays or strings, just signed integers and reals
 * No constant statements, i.e., constants must be initialized with a simple number.
 * No input/output instructions.
 * No interactive mode for debugging; just automatic single stepping (verbose == true)

Versions:
    1.0 - Initial release
    1.1
        * Added Pascal style comments
        * TokenString now returns unknown for an unknow token, or badComment for
          an unterminted comment.
        * Folded factors into expressions
        * Added logical and, orr plus bitwise and, or, xor.
    1.2 - Added halt instruction
    1.3
        * Interp registers are now pl0c::Unsigned
        * Added xpl0c.sh to run regression tests
        * Added Datum, the stack data type, as a step towards adding types to
          pl0c and for unsigned bit operations.
    2.0 - Real and integer data types.
	2.0a
		* Combined integer/unsigned/real instructions into single instructions.
		* Converted Datum to a class from a struct
		* Modified the compilier per the simplified expression grammer.

Author
    Randy Merkel, Slowly but Surly Software.

Copyright
    (c) 2017 Slowly but Surly Software.
    All rights reserved.
