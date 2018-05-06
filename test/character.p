program CharTest() is
type
	A is array [0..9] of character;

var i : integer;
	c : character;
 	a1 : A;
 	a2 : A;

begin
	c := 'x';
	putln(c);	
	a1 := "abcdefghij";		{	fill a1 with "abcd..."			}
 	putln(a1);
	a2 := a1;				{	copies the contents of a1 to a2	}
	putln(a2);
	a1 := "0123456788";		{	fill a1 with "0123..."			}
	putln(a1);				{	while a1 has changed...			}
	putln(a2)				{	... a2 has not!					}
endprog
