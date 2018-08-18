Version | Description
-------: | --------------
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
 0.30	| Added builtin type natural, refactored exceptions, added shift left & right
 0.31	| TBD: add Datum ranges...
 0.3x   | ...
 0.40   | Add GET()
 0.41   | GETB, GETC... -> GET()
 0.42   | Fixed array decls; indexed by ordinals, not simple types.
 0.43   | Add *type*_min, max as pseudo attributes
 0.44   | Add check for functions with no return statements
 0.45   | Add return statement
 0.46   | Replace xxx_min/max with `min, `max attributes.
 ?.??   | Extend const-expressions to include +, /, etc.
