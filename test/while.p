{ Simple while loop test }
program ForTest() is
var
	i : integer;

begin
	i := 0;
	while (i <= 9) loop
		writeln(i);
		i := i + 1
	endloop
endprog