program CharTest() is
type A is array [0..9] of character;

var i : integer;
	c : character;
 	a1 : A;
 	a2 : A;

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
	put_line(a2)
endprog
