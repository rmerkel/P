{ Test advanced types	}
program testtype ;
type
	int = integer;
	rng = 1..127;

var
	i	: integer;
	i2	: int;

	r	: 0..127;
	r2	: rng;

	a	: array [rng] of integer;

begin
	i := 0;
	i := i + 1;

	i2 := 0;
	i2 := i2 + 1;	
	i2 := i2 + i;

	r := 0;
	r := r + 1;

	r2 := 0;
	r2 := r2 + 1;
	r2 := r2 + r

{	a[0] = 0	}
 end.
