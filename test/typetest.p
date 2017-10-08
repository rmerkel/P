{ Test advanced types	}
program typetest;
type
	int = integer;
	rng = 1..10;
	enum = (	one, two, three	);

var
	i : int;
	r : rng;
	a : array[rng] of integer;
	a2 : array [enum] of int;

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
	a2[three] := 3
end .
