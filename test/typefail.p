{ Test advanced types, fail	}
program typefail ;
type
	rng = 0..9;
	enum = (	one, two, three	);

var
	i : Integer;
	r : rng;
	a : array[rng] of Integer;
	a2 : array [enum] of Integer;

begin
	i := one;				{	error: expected Integer, got enum		}

	r := 0;
	while r < 10 do begin 	{	*** warning: condition might always be true	}
		a[r] := r;
		r := r + 1
	end;
	r := 10;				{	*** error: out-of-range/range check error	}
	a[10] := 10;			{	*** error: out-of-range/range check error	}
	a[1+9] := 10;			{	*** error: out-of-range/range check error	}

	a2[one] := 1;
	a2[2] := 2;				{	error: got Integer, expected enum		}
	a2[two + 1] := 3		{	error: expected enum, got Integer		}
end .
