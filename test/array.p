program CharTest()
type
	A = array [0..9] of character;

var i : integer;
	c : character;
 	a1 : A;
 	a2 : A;

begin
	c := 'x';
	writeln(c);	

	a1 := "abcdefghij";		{	fill a1 with "abcd..."			}
 	writeln(a1);

	a2 := a1;				{	copies the contents of a1 to a2	}
	writeln(a2);

	a1 := "0123456789";		{	fill a1 with "0123..."			}
							{	while a1 has changed...			}
	writeln(a1);
							{	... a2 has not!					}
	writeln(a2)
end .
