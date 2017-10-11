{ Test promotion bug if lhs is integer, rhs is real	}
{	emits two itor2 instructions					}
program eval ;
var 
	r : Real;
begin
	r := 1 + 3 * 10.0
end .
