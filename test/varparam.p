{	test var parameters		}
program VarParamTest ;
var	i : integer;
	procedure inc(var x : integer) ;
	begin
		x := x + 1
	end ;
begin
	i := 0;
	inc(i);
    writeln(i)				{	s/b 1, not zero	}
end .

