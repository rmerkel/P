program testif() is
var x, y, z : integer;
begin
	x := 1;
	y := 2;
	z := 3;
	{ set x to 2	}
	if x = 1 then x := y else x := z endif;
	writeln(x);

	{ set x to  3	}
	if x = y then x := z else x := y endif;
	writeln(x)
endprog

