program Fibonacci() is
	function fib(a, b, limit : integer) : integer is
	begin
		writeln(b);
		if limit > 0 then
			fib := fib(b, a+b, limit - 1)
		else
			fib := a + b
		endif
	endfunc

begin
	writeln(0);
	writeln(fib(0, 1, 10))
endprog

