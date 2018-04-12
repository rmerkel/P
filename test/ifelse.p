program IfElseTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		writeln("i is greater than 10");
		i := 1
	else
		writeln("i is <= 10");
		i := 3
	endif;

	writeln("i is ", i)
endprog

