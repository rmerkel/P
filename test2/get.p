program gettest() is
var i : integer;
	r : real;
	ai : array [0..4] of integer;
	ab : array [0..1] of boolean;
	ar : array [1..5] of real;
	s : array [0..9] of character;
begin
	putln("5 integers: ");
	get(ai);
	putln(ai);

	putln("5 reals: ");
	get(ar);
	putln(ar, 0, 7);

	putln("2 booleans: ");
	get(ab);
	putln(ab);

	putln("10 characters: ");
	get(s);
	put('"');
	put(s);
	putln('"');

	putln("integer: ");
	get(i);
	putln(i);

	putln("real: ");
	get(r);
	putln(r, 0, 7)
endprog
