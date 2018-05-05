program IfTest() is
var i : integer;
begin
	i := 10;
	if i > 10 then
		writeln("i is greater than 10");
		i := 1
	endif;

	write("i is ");
	writeln(i)
endprog

