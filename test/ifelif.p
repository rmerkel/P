program IfElifTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		writeln("i is greater than 10");
		i := 1
	elif i < 0 then
		writeln("i is less than 0");
		i := 2
	endif;

	write("i is ");
	writeln(i)
endprog

