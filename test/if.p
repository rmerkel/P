program IfTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		putln("i is greater than 10");
		i := 1
	endif;

	put("i is ");
	putln(i)
endprog

