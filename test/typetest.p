{ Test advanced types	}
program typetest() is
type
	int is integer;
	rng is 1..10;
	enum is (	one, two, three	);

var
	i : int;
	j : integer;
	r : rng;
	a : array[rng] of integer;
	a2 : array [enum] of int;
	a3 : array [0..4] of array [0..4] of real;

begin
	i := 1; i := i + 1;
	r := 1; r := r + 1;

	i := 1;	{	fill a[] with its index	}
 	while i < 11 loop 
		a[i] := i;
		writeln(a[i]);
		i := i + 1
	endloop;

	r := 1;	{	multiply by 10			}
	repeat
		a[r] := a[r] * 10;
		writeln(a[r]);
		r := r + 1
	until r = 10 endloop;

	a2[one]	:= 1;
	a2[two]	:= 2;
	a2[three] := 3;
	writeln(a2[one], a2[two], a2[three]);

	i := 0;	{	fill a3[] with it's index	}
	while (i < 5) loop
		j := 0;
		while (j < 5) loop
			a3[i][j] := 1.0 * (i + j);
			write(a3[i][j] : 7 : 4);
			j := j + 1
		endloop;
		writeln;
		i := i + 1
	endloop
endprog
