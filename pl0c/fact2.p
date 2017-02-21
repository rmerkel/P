var result;
function factorial(n)
	var p;
    begin
        p = 1;
        while n > 1 do begin
            p = p * n;
            n = n - 1
        end;
		factorial = p;
    end;

begin
    result = factorial(10)
end.
