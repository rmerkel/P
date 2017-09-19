{ Test advanced types	}
program testtype ;
type
	int = integer;
	rng = 0..9;

var
	i	: int;
	i2  : integer;
	r	: 0..9;
	r2	: rng;
	a	: array [rng] of int;

begin
	i := 0;
	i := i + 1;

	r := 0;
	r := r + 1;

	r2 := 0;
	r2 := r2 + 1;
	r2 := r2 + r;

	i := 0;
 	while i < 10 do begin 
		a[i] := i;
		i := i + 1
	end;

	i := 0;
 	while i < 10 do begin 
		a[i] := a[i] * 10;
		i := i + 1
	end	
 end.
