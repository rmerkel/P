# The P Programming Language

Yet another small, interpreted, computer language... just for the fun of it!

A compiler/interpreter, inspired by the original PL/0 language and machine
described in "Algorithms + Data Structures = Programs," 1st Edition, by Wirth.
Currently, the compiler supports a dialect somewhere between PL/0 and ISO
Pascal, but may *never* be fully ISO compatible. 

Current plans are to start to diverge from Pascal, for example:
 * Allow pointers to point to any variable, thus pass by reference would be 
   similar to C; pass a pointer.
 * Add object attributes, similar to ADA
 * Bit operators in place of Pascal sets
...

Like PL/0, P is a combination compiler and interpreter; it first runs the
compiler (PComp), and if no errors where encountered, it runs the results in
the interpreter (PInterp). A listing and machine output are written to
standard output.

The compiler started life as a copy of the C example;
[Recursive Descent Parser](https://en.wikipedia.org/wiki/Recursive_descent_parser/), 
modified to emit code per Wirth's interpret procedure while using the
TokenStream (TokenStream) from "The C++ Programming Language," 4th Edition, by 
Stroustrup, modified to support the PL/0 dialect, and thus Pascal. By default, 
PComp writes error messages on standard error output. The listing (-l) option
writes a listing on standard output, and and the verbose (-v) option logs tokens
found and code emitted.

The machine/interpreter stated life as a C/C++ port of Wirth's machine
(interpret procedure), modified to use lest "weird" instruction names,
modified again to use indirect addressing per HOC, as described in "The UNIX
Programming Environment," by Kernighan and Pike. By default, PInterp logs
"assigns" on standard output, but the verbose (-v) option will run the program
in trace mode; disassembling and dumping the activation frame for each
instruction single step the program.

## Release
* git add .
* git commit -m "description..."
* git tag -a vMajor.Minor -m "summary..."
* git push origin branch-name

Note that global push.followTags has been set true, so --tags *should not* be 
necessary.

## Bugs
* No files.
* No input instructions, and just elementary Write/Writeln implementation.
* No interactive mode for debugging; just automatic single stepping (-v)

## History

Version | Description
------: | --------------
  0.1   | Initial fork form pl0c, modified to use =, <> etc.
  0.2   | Program declaraction
  0.3   | Removed the C/C++ bit and shift operations
  0.4   | Added types, limited to ranges and arrays.
  0.5   | Added enumerations
  0.6   | Fixed array index type checks
  0.7   | Supports non-zero based array indexes
  0.8   | Supports multiple dimensioned arrays. Fixed promotion bug
  0.9   | Supports a[i,j] and a[i][j] syntax
 0.10   | Adding built-in functions and constants, refactored types, capitalized built-in "nouns" and functions. WIP.
 0.11   | Primitive Write and Writeln, refactoring.
 0.12   | Limit checks, successor, predecessor. for statement.
 0.13   | Refactored TDesc into iTDesc and it's derived classes
 0.14   | Write/WriteLn field specifiers.
 0.15   | Records and selectors
 0.16   | Refactored records, selectors and arrays. Added New(), Dispose() and nil
 0.16   | Boolean
 0.17	| Arrays are now passed by value correctly.
 0.18	| Writeln now prints "true" "false" for booleans.
 0.19   | Clean up, removed unused global data/constants, use input filename in messages
 0.20	| Refactored the interpreter step routine, adding more run-time tests and -t option
 0.21   | Write and writeln nolonger quote character values. Major refactor of Datum.
 0.22   | Write[ln] now supports arrays and strings.
 0.23   | Call and jump indirection; target address is on the stack
 0.24	| Added -l[listing], fixed fahr*.p tests, added call and jump immediate instructions.
 0.25	| Table of pointers to instructions
 0.26   | 'var' parameters; pass by reference
 0.27	| Fixed typo in varparam.p
 0.28	| Bit-wise operations
 0.29	| Manditory '()' on procedure/function declaractions and calls.
 0.30	| Added builtin type natural; a range of 0..maxint

## Design and implementation notes

* To support passing strings, or more generally, arrays to write[ln], a count
  is passed with each parameter, 1 for scalers, N for the string or array
  length. 
* Opening and closing brackets "()" are mandatory for subroutine declarations
  and calls.
* Currently supports "var" subroutine parameter types for pass by reference, but
  pointers might be a better choice. 
* No set operations, but using bitwise operators, bit_and/or/xor/not in their
  place.
* Currently, the interpreter uses a memory model somewhere between fully tagged,
  and a simple. While Datum directly supports simple types, there is no support 
  for user defined types. This requires the compiler to generate limit check
  instructions. If Datums contained a TypeDescPtr, the interpreter could could 
  make range check errors directly.

## Author
    Randy Merkel, Slowly but Surly Software.

## Copyright
    (c) 2018 Slowly but Surly Software.
    All rights reserved.
