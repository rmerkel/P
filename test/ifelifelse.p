program IfElifElseTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		putln("i is greater than 10");
		i := 1
	elif i < 0 then
		putln("i is less than 0");
		i := 2
	else
		put("i is between 1 and 10, inclusive ");
		putln(i);
		i := 3
	endif;

	put("i is ");
	putln(i)
endprog

