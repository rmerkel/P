program IfElifElseTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		writeln("i is greater than 10");
		i := 1
	elif i < 0 then
		writeln("i is less than 0");
		i := 2
	else
		writeln("i is between 1 and 10, inclusive", i);
		i := 3
	endif;

	writeln("i is ", i)
endprog

