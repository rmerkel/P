program TestBitWise() is
var	x, y, z : integer ;
begin
	x := 15;
	y := 1;
	put("x = "); put_line(x);
	put("y = "); put_line(y);

	z := x bit_and y;
	put("z := x bit_and y = ");
	put_line(z);

	z := 10 bit_or z;
	put("z :+ 10 bit_or z = ");
	put_line(z);

	z := x bit_xor y;
	put("z := x bit_xor y = ");
	put_line(z);

	z := bit_not z;
	put("bit_not z = ");
	put_line(z);

	z := x shift_left y;
	put("z := x shift_left y = ");
	put_line(z);

	z := x shift_right y;
	put("z := x shift_right y = ");
	put_line(z)
endprog
