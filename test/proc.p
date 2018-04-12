program ProcTest()
var	i : integer;
procedure Proc(i : integer; r : real; b : boolean)
	begin
		writeln(i, r, b);
	end ;

begin
	Proc(2.5, 3, false);
end;
