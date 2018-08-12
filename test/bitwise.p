program TestBitWise() is
var	x, y, z : integer ;
begin
	x := 15;
	y := 1;
	put("x = "); put_line(x);
	put("y = "); put_line(y);

	z := x band y;
	put("z := x band y = ");
	put_line(z);

	z := 10 bor z;
	put("z :+ 10 bor z = ");
	put_line(z);

	z := x bxor y;
	put("z := x bxor y = ");
	put_line(z);

	z := bnot z;
	put("bnot z = ");
	put_line(z);

	z := x sleft y;
	put("z := x sleft y = ");
	put_line(z);

	z := x sright y;
	put("z := x sright y = ");
	put_line(z)
endprog
