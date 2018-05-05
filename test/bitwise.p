program TestBitWise() is
var	x, y, z : integer ;
begin
	x := 15;
	y := 1;
	write("x = "); writeln(x);
	write("y = "); writeln(y);

	z := x bit_and y;
	write("z := x bit_and y = ");
	writeln(z);

	z := 10 bit_or z;
	write("z :+ 10 bit_or z = ");
	writeln(z);

	z := x bit_xor y;
	write("z := x bit_xor y = ");
	writeln(z);

	z := bit_not z;
	write("bit_not z = ");
	writeln(z);

	z := x shift_left y;
	write("z := x shift_left y = ");
	writeln(z);

	z := x shift_right y;
	write("z := x shift_right y = ");
	writeln(z)
endprog
