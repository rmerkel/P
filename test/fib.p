program Fibonacci() is
	function fib(a, b, limit : integer) : integer is
	begin
		putln(b);
		if limit > 0 then
			return fib(b, a+b, limit - 1)
		else
			return a + b
		endif
	endfunc

begin
	putln(0);
	putln(fib(0, 1, 10))
endprog

