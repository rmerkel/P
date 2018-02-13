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

program fact ;
const nFacts = 10;
var p : integer;
procedure factorial(n : integer) ; begin
        p := 1;
		writeln(p);
        while n > 0 do begin
            p := p * n;
            n := n - 1;
			writeln(p)
        end
    end;

begin
    factorial(nFacts)
end.
