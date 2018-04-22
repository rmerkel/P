{ Test records }
program RecordTests() is
type
	R is record 
		i1, i2 : integer;
		r : real
	end;
	R2 is record
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
	put(x.i1);
	put(x.i2);
	put_line(x.r, 8, 6);

	y.i1 := 4;
	y.i2 := 5;
	y.r := 6.0;
	put(x.i1);
	put(x.i2);
	put_line(x.r, 8, 6);

	z.i1 := 7;
	z.i2 := 8;
	z.r := 9.0;
	put(x.i1);
	put(x.i2);
	put_line(x.r, 8, 6)
endprog
