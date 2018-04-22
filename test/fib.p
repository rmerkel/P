program Fibonacci() is
	function fib(a, b, limit : integer) : integer is
	begin
		put_line(b);
		if limit > 0 then
			fib := fib(b, a+b, limit - 1)
		else
			fib := a + b
		endif
	endfunc

begin
	put_line(0);
	put_line(fib(0, 1, 10))
endprog

