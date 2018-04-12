{ Test advanced types, fail	}
program Typefail()
type
	rng is 0..9;
	enum is (	one, two, three	);

var
	i : integer;
	r : rng;
	a : array[rng] of integer;
	a2 : array [enum] of integer;

begin
	i := one;				{	error: expected integer, got enum		}

	r := 0;
	while r < 10 loop 		{	*** warning: condition might always be true	}
		a[r] := r;
		r := r + 1
	endloop;
	r := 10;				{	*** error: out-of-range/range check error	}
	a[10] := 10;			{	*** error: out-of-range/range check error	}
	a[1+9] := 10;			{	*** error: out-of-range/range check error	}

	a2[one] := 1;
	a2[2] := 2;				{	error: got integer, expected enum		}
	a2[two + 1] := 3		{	error: expected enum, got integer		}
end;
