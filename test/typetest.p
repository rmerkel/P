{ Test advanced types	}
program typetest;
type
	int = integer;
	rng = 0..9;
	enum = (	one, two, three	);

var
	i : int;
	r : rng;
	a : array[rng] of integer;
	a2 : array [enum] of int;

begin
	i := 0; i := i + 1;
	r := 0; r := r + 1;

	i := 0;	{	fill a[] with its index	}
 	while i < 10 do begin 
		a[i] := i;
		i := i + 1
	end;

	r := 0;	{	multiply by 10			}
	repeat begin
		a[r] := a[r] * 10;
		r := r + 1
	end until r = 9;

	a2[one]	:= 1;
	a2[two]	:= 2;
	a2[three] := 3
end .
