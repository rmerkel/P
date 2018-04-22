program IfElseTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		put_line("i is greater than 10");
		i := 1
	else
		put_line("i is <= 10");
		i := 3
	endif;

	put("i is ");
	put_line(i)
endprog

