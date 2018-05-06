{ Simple for loop test	}
program ForTest() is
var
	i : integer;

begin
	for i in reverse 0..9 loop
		putln(i)
	endloop
endprog
