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
var n, f;	
begin
   n = 0;
   f = 1;
   while n < nFacts do begin
      n = n + 1;
      f = f * n
   end
end.

