program precedence ;
var x : integer;
begin
	x := 1 + 2 * 3 - 4;	{	s/b 3	}
	x := -1 + 2 * 3 - 4	{	s/b 1	}
end .
