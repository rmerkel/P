program Fibonacci()
	function fib(a, b, limit : integer) : integer
	begin
		writeln(b);
		if limit > 0 then
			fib := fib(b, a+b, limit - 1)
		else
			fib := a + b
		endif
	end ;

begin
	writeln(0);
	writeln(fib(0, 1, 10))
end ;

