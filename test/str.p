program StringTest() is
type
	A is array [0..9] of character;

var
 	a1 : A;
 	a2 : A;
	i : integer;

begin
	a1 := "abcdefghij";		{	fill a1 with "abcd..."			}
	a2 := a1; 				{	copies the contents of a1 to a2	}

	put_line("for i in 0..9...");
	for i in 0..9 loop put_line(a2[i]) endloop;

	put_line("for i in A...");
	for i in A loop put_line(a1[i]) endloop	{	;

	put_line("for i in a1...");
	for i in a1 loop put_line(a1[i]) endloop	}
endprog
