{ test builtin functions and procedures	}
program builtins ;
const
	pi = 3.1415926;

type
	e = (zero, one, two, three);

var
	i : integer; 
	r : real;

begin
	i := 1;					{ result: 1			}
	i := round(2.0);		{ result: 2			}
	i := round(2.5);		{ result: 3 		}
	i := trunc(4.0);		{ result: 4 		}
	i := trunc(5.9);		{ result: 5 		}
	i := maxint;			{ result  2,147,483,647	}
	
	i := abs(1);			{ result: 1			}
	i := abs(-1);			{ result: 1			}
	r := abs(1.5);			{ result 1.5		}
	r := abs(-1.5);			{ result 1.5		}

	i := ord(three);		{ result: 3			}
	i := ord(i-1);			{ result: 2			}

{	i := ord(1.2)			error: ordinal value expected	}

	r := arctan(1);			{ result:  0.785398	}
	r := arctan(1.0);		{ result:  0.785398	}

	r := exp(1);			{ result: 2.718282	}
	r := exp(1.0);			{ result: 2.718282	}

	r := ln(1);				{ result: 0.0		}
	r := ln(1.0);			{ result: 0.0		}

	i := odd(10);			{ result: 0			}
	i := odd(10+1);			{ result: 1			}

	r := sin(-3*pi/4);		{ result: -0.707107	}
	r := sqr(10);			{ result: 100		}
	r := sqrt(2);			{ result: 1.41421	}

	writeln(i, r, pi);
	writeln(pi : 7 : 5);
	writeln
end .

