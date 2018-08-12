program CharTest() is
type A is array [0..9] of character;

var i : integer;
	c : character;
 	a1 : A;
 	a2 : A;
	ai : array [0..9] of integer;
	ar : array [0..9] of real;

begin
	c := 'x';
	put_line(c);	

	a1 := "abcdefghij";		{	fill a1 with "abcd..."			}
 	put_line(a1);

	a2 := a1;				{	copies the contents of a1 to a2	}
	put_line(a2);

	a1 := "0123456789";		{	fill a1 with "0123..."			}
							{	while a1 has changed...			}
	put_line(a1);
							{	... a2 has not!					}
	put_line(a2);

	for i in 0..9 loop
		ai[i] := i
	endloop;
	put_line(ai);

	for i in 0..9 loop
		ar[i] := i * 1.1;
	endloop;
	put_line(ar);
	put_line(ar,4,1)
endprog
