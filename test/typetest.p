{ Test advanced types	}
program typetest;
type
	int = Integer;
	rng = 1..10;
	enum = (	one, two, three	);

var
	i : int;
	j : Integer;
	r : rng;
	a : array[rng] of Integer;
	a2 : array [enum] of int;
	a3 : array [0..4] of array [0..4] of Real;

begin
	i := 1; i := i + 1;
	r := 1; r := r + 1;

	i := 1;	{	fill a[] with its index	}
 	while i < 11 do begin 
		a[i] := i;
		i := i + 1
	end;

	r := 1;	{	multiply by 10			}
	repeat begin
		a[r] := a[r] * 10;
		r := r + 1
	end until r = 11;

	a2[one]	:= 1;
	a2[two]	:= 2;
	a2[three] := 3;

	i := 0;	{	fill a3[] with it's index	}
	while (i < 5) do begin
		j := 0;
		while (j < 5) do begin
			a3[i][j] := 1.0 * (i + j);
			j := j + 1
		end;
		i := i + 1
	end
end .
