program TestBitWise() is
var	x, y, z : integer ;
begin
	x := 15;
	y := 1;
	put("x = "); putln(x);
	put("y = "); putln(y);

	z := x band y;
	put("z := x band y = ");
	putln(z);

	z := 10 bor z;
	put("z :+ 10 bor z = ");
	putln(z);

	z := x bxor y;
	put("z := x bxor y = ");
	putln(z);

	z := bnot z;
	put("bnot z = ");
	putln(z);

	z := x sleft y;
	put("z := x sleft y = ");
	putln(z);

	z := x sright y;
	put("z := x sright y = ");
	putln(z)
endprog
