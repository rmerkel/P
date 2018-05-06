{	test var parameters		}
program VarParamTest() is
var	i : integer;
	procedure inc(var x : integer) is
	begin
		x := x + 1
	endproc
begin
	i := 0;
	inc(i);
    putln(i)				{	s/b 1, not zero	}
endprog

