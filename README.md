# The P Programming Language

Yet another small, interpreted, computer language... just for the fun of it!

A compiler/interpreter, inspired by the original PL/0 language and machine
described in "Algorithms + Data Structures = Programs," 1st Edition, by Wirth.
Originally, the goal was a ISO compatible Pascal complier. P progressed to a
point somewhere between PL/0 and ISO Pascal before this goal was abandoned.

Current plans is to diverge from Pascal, for a sort of "ADA Lite," but with 
other differences, for example;
 * Allow pointers to point to any variable, thus pass by reference would be 
   similar to C; pass a pointer.
 * Add object attributes, similar to ADA
 * Bit operators in place of Pascal sets

Like PL/0, P is a combination compiler and interpreter; it first runs the
compiler (PComp), and if no errors where encountered, it runs the results in
the interpreter (PInterp). A listing and machine output are written to
standard output.

The compiler started life as a copy of the C example;
[Recursive Descent Parser]
(https://en.wikipedia.org/wiki/Recursive_descent_parser/), modified to emit code
per Wirth's interpret procedure while using the TokenStream (TokenStream) from
"The C++ Programming Language," 4th Edition, by Stroustrup, modified to support
the PL/0 dialect, and thus Pascal. By default, PComp writes error messages on 
standard error output. The listing (-l) option writes a listing on standard
output, and and the verbose (-v) option logs tokens found and code emitted.

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

## Design notes and goals
 * Single quotes for character literals, double for strings.
 * There is just one basic integer type, the signed Integer. 
 * Unsigned integers (Natural), is a built-in subrange of Integer; 0..maxint, 
   the maximum Integer value; thus any Natural value will 'fit' in a Integer.
   Likewise, Positive is 1..maxint.
 * Boolean is a built-in enumeration of false, true [0, 1]
 * Character is a subrange of 0..127.
 * Subrange checking is accomplished via the limit check instructions, LLIMIT
   and ULIMIT when for assignments from a ordinal value with a wider range to a
   narrower one. This should be omitted in the case of constant expressions
   where the range of the value is know... but currently isn't.
 * Variables do not have default values; they must be explicitly initialized by
   via assignment, otherwise, the compiler would have to emit initialization
   instructions on entry to every new block.
 * Either the compiler or the machine could emit errors if a uninitialized 
   variable is used. Alternatively, variables could have default values.
 * Allow pointers to point to any variable, thus pass by reference would be 
   similar to C, e.g, pass a pointer.
 * Add object attributes, similar to ADA.
 * Bit operators in place of Pascal sets; bit_and, bit_or, ..., shift_left,
   shift_right.
 * Automatically generate *type*_min, max, in place of attributes.
 * Return statement is similar to pascals to func := expr, followed by return 
   instruction. 
   * For functions, this would replace the default RETF instruction generated at
     the end of the function, but introduces the need to detect functions with
     no return statements. Otherwise the function might "fall out" of the
     function.
   * Likewise, one can return early from a procedure, resulting in two RET 
     statements, e.g, "procedure foo() begin ... return endproc." However, that
     won't result in an erroneous program.

## Configuration
* Configure cc/c++ as clang:
```
	sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100
	sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100
```
* Configure cc/C++ as gcc:
```
	sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 100
	sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 100
```

## Author
    Randy Merkel, Slowly but Surly Software.

## Copyright
    (c) 2018 Slowly but Surly Software.
    All rights reserved.
