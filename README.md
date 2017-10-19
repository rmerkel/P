Pascal-lite Programming Language

Yet another small, interpreted, computer language... just for the fun of it!

A compiler/interpreter, inspired by the original PL/0 language and machine
described in Algorithms + Data Structures = Programs, 1st Edition, by Wirth.
Currently, the compilier supports a dialect between PL/0 and Pascal.

Like PL/0, pas is a combination compiler and interpreter; it first runs the
compiler (Comp), and if no errors where encountered, it runs the results in
the interpreter (Interp). A listing and machine output are written to
standard output.

The compiler started life as a copy of the C example at
https://en.wikipedia.org/wiki/Recursive_descent_parser, modified to emit code 
per Wirth's interpret procedure while using the TokenStream (TokenStream) from 
The C++ Programming Language, 4th Edition, by Stroustrup, modified to support 
the PL/0 dialect, and thus Pascal. By default, Comp writes a listing to
standard output, but the verbose (-v) option will also log tokens found and 
code emitted.

The machine/interpreter stated life as a C/C++ port of Wirth's machine
(procedure interpret), modified to use lest "weird" instruction names, modified
again to use indirect addressing per HOC, as described in The UNIX Programming
Environment, by Kernighan and Pike. By default, Interp logs "assigns" on
standard output, but the verbose (-v) option will run the program in trace
mode; disassembling and dumping the activation frame for each instruction
single step the program.

Release
 * git add .
 * git commit -m "description..."
 * git tag -a vMajor.Minor -m "summary..."
 * git push --tags origin branch-name

Bugs
 * More pl0c than Pascal at this point
 * No strings, just signed integers and reals
 * No input instructions, and just elementary Write/Writeln implementation.
 * No interactive mode for debugging; just automatic single stepping (-v)

Versions
 * 0.1  - Initial fork form pl0c, modified to use =, <> etc.
 * 0.2  - Program declaraction
 * 0.3  - Removed the C/C++ bit and shift operations
 * 0.4  - Added types, limited to ranges and arrays.
 * 0.5  - Added enumerations
 * 0.6  - Fixed array index type checks
 * 0.7  - Supports non-zero based array indexes
 * 0.8  - Supports multiple dimensioned arrays. Fixed promotion bug
 * 0.9  - Supports a[i,j] and a[i][j] syntax
 * 0.10 - Adding built-in functions and constants, refactored types, capitalized built-in "nouns" and functions. WIP.
 * 0.11 - Primitive Write and Writeln, refactoring.
 * 0.12 - Limit checks, successor, predecessor. for statement.

Author
    Randy Merkel, Slowly but Surly Software.

Copyright
    (c) 2017 Slowly but Surly Software.
    All rights reserved.
