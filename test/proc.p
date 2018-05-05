program ProcTest() is
var	i : integer;
procedure Proc(i : integer; r : real; b : boolean) is
	begin
		write(i);
		write(r);
		writeln(b)
	endproc

begin
	Proc(2.5, 3, false)
endprog
