{ Test advanced types	}
program testtype ;
	{	integers, ranges and aliases	}
	procedure test1;
	type
		int = integer;
		rng = 0..9;
	var
		i	: int;
		i2	: integer;
		r	: 0..9;
		r2	: rng;
	begin
		i := 0;
		i := i + 1;

		i2 := 0;
		i2 := i2 + 1;

		r := 0;
		r := r + 1;
{		r := 10;			compilier error: out of range	}

		r2 := 0;
		r2 := r2 + 1;
		r2 := r2 + r
	end;

	{	arrays	}
	procedure test2;
	type
		rng = 0..9;
	var
		i : integer;
		ir : rng;
		a : array[rng] of integer;
	begin
		i := 0;
 		while i < 10 do begin 
			a[i] := i;
			i := i + 1
		end;

		ir := 0;
		repeat begin
			a[ir] := a[ir] * 10;
			ir := ir + 1
		end until ir = 10	{	compilier warning: may always be false	}
	end;

	procedure test3;
	type
		enum = (	one, two, three	);
	var
		a2 : array [enum] of integer;
	begin
		a2[one] := 1;
		a2[two] := 2;
		a2[three] := 3		{	ord(three)	}
	end;

begin
	test1;
	test2;
	test3
end.
