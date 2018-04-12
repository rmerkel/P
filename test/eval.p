{ Test promotion bug if lhs is integer, rhs is real	}
{	emits two itor2 instructions					}
program eval() is
var 
	r : real;
begin
	r := 1 + 3 * 10.0;
	writeln(r)
endprog
