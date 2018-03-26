program TestBitWise()
var	x, y, z : integer ;
begin
	x := 15;
	y := 1;
	writeln('x = ', x);
	writeln('y = ', y);

	z := x bit_and y;
	writeln('z := x bit_and y = ', z);

	z := 10 bit_or z;
	writeln('z :+ 10 bit_or z = ', z);

	z := x bit_xor y;
	writeln('z := x bit_xor y = ', z);

	z := bit_not z;
	writeln('bit_not z = ', y)

end .
