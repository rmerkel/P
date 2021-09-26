program TestBitWise() is
var	x, y, z : integer ;
begin
	x := 15;
	y := 1;
	put("x = "); putln(x);
	put("y = "); putln(y);

	z := x bit_and y;
	put("z := x bit_and y = ");
	putln(z);

	z := 10 bit_or z;
	put("z :+ 10 bit_or z = ");
	putln(z);

	z := x bit_xor y;
	put("z := x bit_xor y = ");
	putln(z);

	z := bit_not z;
	put("bit_not z = ");
	putln(z);

	z := x bit_sleft y;
	put("z := x bit_sleft y = ");
	putln(z);

	z := x bit_sright y;
	put("z := x bit_sright y = ");
	putln(z)
endprog
