program FunctionNoReturn() is
const nFacts = 10;
var result : integer;
function factorial(n : integer) : integer is
	var p : integer;
	begin
		p := 1;
		while n > 0 loop
			p := p * n;
			n := n - 1
		endloop;
{		factorial := p			Omit any return statement!	}
	endfunc

begin
	{ The result is the 10th factorial; 3,628,000	}
    result := factorial(nFacts);
	putln(result)
endprog
