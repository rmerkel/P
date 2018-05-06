program Fibonacci() is
	function fib(a, b, limit : integer) : integer is
	begin
		putln(b);
		if limit > 0 then
			fib := fib(b, a+b, limit - 1)
		else
			fib := a + b
		endif
	endfunc

begin
	putln(0);
	putln(fib(0, 1, 10))
endprog

