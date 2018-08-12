program testif() is
var x, y, z : integer;
begin
	x := 1;
	y := 2;
	z := 3;
	{ set x to 2	}
	if x = 1 then x := y else x := z endif;
	put_line(x);

	{ set x to  3	}
	if x = y then x := z else x := y endif;
	put_line(x)
endprog

