program StringTest()
type
	A = array [0..9] of character;

var
 	a1 : A;
 	a2 : A;
	i : integer;

begin
	a1 := "abcdefghij";		{	fill a1 with "abcd..."			}
	a2 := a1; 				{	copies the contents of a1 to a2	}
	for i := 0 to 9 do writeln(a2[i])
end .
