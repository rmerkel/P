{ Calculate 11 (0..10) factorials
{	 n		    n!	}
{  ---  ---------	}
{	 0		    0	}
{	 1		    1	}
{	 2		    2	}
{	 3		    6	}
{	...				}
{	 9	  362,880	}
{	10	3,628,800	}

const nFacts = 10;
var result;
function factorial(n)
	var p;

	begin
		p = 1;
		while n > 0 do begin
			p = p * n;
			n = n - 1
		end;
		factorial = p;
	end;

begin
	{ call with wrong number of parameters! }
    result = factorial(nFacts, nFacts)
end.
