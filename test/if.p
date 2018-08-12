program IfTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		put_line("i is greater than 10");
		i := 1
	endif;

	put("i is ");
	put_line(i)
endprog

