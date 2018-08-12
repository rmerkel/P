program gettest() is
var i : integer;
	r : real;
	ai : array [0..4] of integer;
	ab : array [0..1] of boolean;
	ar : array [1..5] of real;
	s : array [0..9] of character;
begin
	put_line("5 integers: ");
	get(ai);
	put_line(ai);

	put_line("5 reals: ");
	get(ar);
	put_line(ar, 0, 7);

	put_line("2 booleans: ");
	get(ab);
	put_line(ab);

	put_line("10 characters: ");
	get(s);
	put('"');
	put(s);
	put_line('"');

	put_line("integer: ");
	get(i);
	put_line(i);

	put_line("real: ");
	get(r);
	put_line(r, 0, 7)
endprog
