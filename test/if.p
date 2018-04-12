program IfTest()
var i : integer;
begin
	i := 10;
	if i > 10 then
		writeln("i is greater than 10");
		i := 1
	endif;

	writeln("i is ", i)
end;

