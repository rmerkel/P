{ Calculate 11 (0..10) factorials	}
{	 n		    n!	}
{  ---  ---------	}
{	 0		    0	}
{	 1		    1	}
{	 2		    2	}
{	 3		    6	}
{	...				}
{	 9	  362,880	}
{	10	3,628,800	}

program fact2() is
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
		factorial := p
	endfunc

begin
	{ The result is the 10th factorial; 3,628,000	}
    result := factorial(nFacts);
	putln(result)
endprog
