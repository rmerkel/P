{ Test records }
program RecordTests()
type
	R = record 
		i1, i2 : integer;
		r : real
	end;
	R2 = record
		i1, i2 : integer;
		r : real
	end;

var
	x : R;
	y : R2;
	z : record
		i1, i2 : integer;
		r : real
	end;

begin
	x.i1 := 1;
	x.i2 := 2;
	x.r := 3.0;
	writeln(x.i1, x.i2, x.r : 8 : 6);

	y.i1 := 4;
	y.i2 := 5;
	y.r := 6.0;
	writeln(x.i1, x.i2, x.r : 8 : 6);

	z.i1 := 7;
	z.i2 := 8;
	z.r := 9.0;
	writeln(x.i1, x.i2, x.r : 8 : 6)
end .
