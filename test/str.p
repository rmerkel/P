program StringTest ;
type
	A = array [0..9] of char;

var
 	a1 : A;
 	a2 : A;

begin
	a1 := 'abcdefghij';		{	fill a1 with 'abcd...'			}
	a2 := a1 				{	copies the contents of a1 to a2	}
end .
