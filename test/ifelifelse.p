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
		write("i is between 1 and 10, inclusive ");
		writeln(i);
		i := 3
	endif;

	write("i is ");
	writeln(i)
endprog

