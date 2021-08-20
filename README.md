# The P Programming Language

Yet another small, interpreted, computer language... just for the fun of it!

A compiler/interpreter, inspired by the original PL/0 language and machine
described in "Algorithms + Data Structures = Programs," 1st Edition, by Wirth.
Originally, the goal was a ISO compatible Pascal compiler. P progressed to a
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
(https://en.wikipedia.org/wiki/Recursive_descent_parser), modified to emit code
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

## The P EBNF grammer

See [Extended Backus Naur Form](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form)

              program = "program" ident param-lst "is" block-decl "endprog" ;
           block-decl = [ const-decl-lst ';' ]
                        [ var-decl-blk ';'   ]
                        [ sub-decl-lst ';'   ]
                        "begin" stmt-lst "end" ;
       const-decl-lst = "const" const-decl { ';' const-decl } ;
           const-decl = ident '=' const-expr ;
        type-decl-lst = type-decl { ';' type-decl } ;
            type-decl = "type" "is" type ;
         var-decl-blk = "var" var-decl-lst ;
         var-decl-lst = var-decl { ';' var-decl } ;
             var-decl = ident-lst : type ;
                 type = simple-type | structured-type | pointer-type ;
          simple-type = "real" | ordinal-type ;
     ordinal-type-lst = ordinal-type { ',' ordinal-type } ;
         ordinal-type = '(' ident-lst ')' | const-expr ".." const-expr |
                        "boolean" | "integer" | "natual" | "positive" | "character" ;
      structured-type = "array" '[' ordinal-type-lst ']' "of" type |
                        "record" field-lst "end" ;
            field-lst = var-decl-lst ;
         pointer-type = '^' ident ;
         sub-decl-lst = func-decl | proc-decl ;
            proc-decl = "procedure" ident param-lst "is" block-decl "endproc" ;
            func-decl = "function"  ident param-lst ':' type "is" block-decl "endfunc" ;
            param-lst = '(' [ param-decl-lst ] ')' ;
       param-decl-lst = param-decl { ';' param-decl } ;
           param-decl = [ "var" ] ident-lst : type ;
            ident-lst = ident { ',' ident } ;
             stmt-lst = stmt {';' stmt }  ;
              if-stmt = "if" expr "then" stmt-lst
                           { "elif" expr "then" stmt-lst }
                           [ "else" stmt-lst ]
                           "endif" ;
           while-stmt = "while" expr "loop" stmt-lst "endloop" ;
             for-stmt = "for" ident "in" [ "reverse" ] ordinal-type
                           "loop" stmt "endloop" ;
          return-stmt = "return" [ expr ] ;
                 stmt = [  variable ':=' expr                   |
                           ident '(' [ expr-lst ] ')'           |
                           if-stmt                              |
                           while-stmt                           |
                           "repeat" stmt "until" expr endloop   |
                           for-stmt                             |
                           return-stmt ] ;
           const-expr = [ '+' | '-' ] number | ident | character | string ;
           const-expr = const-simple-expr { expr-op const-simple-expr } ;
    const-simple-expr = [ simple-expr-pre] const-term { simple-expr-op const-term } ;
           const-term = const-fact { term-op  const-fact } ;
           const-fact = ident | attribute | number | '(' const-expr ')' ;
             expr-lst = expr { ',' expr } ;
                 expr = simple-expr { expr-op simple-expr } ;
              expr-op = '<' | "<=" | '=' | ">=" | '>' | "<>" simple-expr ;
          simple-expr = [ simple-expr-pre ] terminal { simple-expr-op terminal } ;
      simple-expr-pre = '+' | '-' | "bnot" ;
       simple-expr-op = '+' | '-' | "bor" | "bxor" | "sleft" | "sright" | "or' ;
             terminal = fact { term-op  fact } ;
              term-op = '*' | '/' | "rem" | "band" | | "and" ;
               fact = variable | attribute | character | string |
                        ident '(' [ expr-lst ] ')'              |
                        "round" '(' expr ')'                    |
                        number                                  |
                        '(' expr ')' ;
             variable = ident [ composite-desc { composite-desc } ] ;
       composite-desc = '[' expr-lst ']'                        |
                        '.' ident                               |
                        '^' ident ;
            attribute = ident '`' ident ;
            character = nul | sox | .. | '}' | '`' | del ;
               string = '"' { character } '"' ;
               number = whole-number | floting-point-number ;
                ident = [ '_' ] { alpha-numeric-character } ;

     {} Repeat zero or more times
     [] Optional; zero or *one* times
     () Grouping
      | One of ...
      ; End of production

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

## Built-ins
### Functions
TBD
### Procedures
TBD

## Design notes and goals
 * Single quotes for character literals, double for strings.
 * There is just one basic integer type, the signed Integer.
 * Unsigned integers (Natural), is a built-in sub-range of Integer; 0..maxint,
   the maximum Integer value; thus any Natural value will 'fit' in a Integer.
   Likewise, Positive is 1..maxint.
 * Boolean is a built-in enumeration of false, true [0, 1]
 * Character is a sub-range of 0..127.
 * Sub-range checking is accomplished via the limit check instructions, LLIMIT
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
 * In place of object attributes (ADA), add more built-ins, e.g., min(integer)
   and not integer'min.
 * Add object attributes, similar to ADA... but
 * Bit operators in place of Pascal sets; bit_and, bit_or, ..., shift_left,
   shift_right.
 * Return statement is similar to pascals to func := expr, followed by return
   instruction.
   * For functions, this would replace the default RETF instruction generated at
     the end of the function, but introduces the need to detect functions with
     no return statements. Otherwise the function might "fall out" of the
     function.
   * Likewise, one can return early from a procedure, resulting in two RET
     statements, e.g, "procedure foo() begin ... return endproc." However, that
     won't result in an erroneous program.
 * **Type attributes**, such as integer`max, are similar to constant expressions, 
   a factor in grammer. However, **variable attributes**, such as i'min are more
   troublesome as variable productions always emit a l-value followed by an
   evaluation instruction, which would require storing the attribute Datum
   somewhere. One possibility is to add "typeof(expr)," which yeilds the type of
   expr without emitting any code to evaluate the expr.

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
