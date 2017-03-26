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

var n, f : integer;
begin
	n = 0;
	f = 1;
	repeat
		begin
			n = n + 1;
			f = f * n
		end
	until n >= 10
end.
