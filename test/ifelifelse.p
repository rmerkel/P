program IfElifElseTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		put_line("i is greater than 10");
		i := 1
	elif i < 0 then
		put_line("i is less than 0");
		i := 2
	else
		put("i is between 1 and 10, inclusive ");
		put_line(i);
		i := 3
	endif;

	put("i is ");
	put_line(i)
endprog

