{ test builtin functions and procedures	}
program builtins ;
const
	pi = 3.1415926;

type
	e = (zero, one, two, three);

var
	i : Integer; 
	r : Real;

begin
	i := 1;					{ result: 1			}
	i := Round(2.0);		{ result: 2			}
	i := Round(2.5);		{ result: 3 		}
	i := Trunc(4.0);		{ result: 4 		}
	i := Trunc(5.9);		{ result: 5 		}
	i := MaxInt;			{ result 2**32		}
	
	i := Abs(1);			{ result: 1			}
	i := Abs(-1);			{ result: 1			}
	r := Abs(1.5);			{ result 1.5		}
	r := Abs(-1.5);			{ result 1.5		}

	i := Ord(three);		{ result: 3			}
	i := Ord(i-1);			{ result: 2			}

{	i := Ord(1.2)			error: ordinal value expected	}

	r := ArcTan(1);			{ result:  0.785398	}
	r := ArcTan(1.0);		{ result:  0.785398	}

	r := Exp(1);			{ result: 2.718282	}
	r := Exp(1.0);			{ result: 2.718282	}

	r := Ln(1);				{ result: 0.0		}
	r := Ln(1.0);			{ result: 0.0		}

	i := Odd(10);			{ result: 0			}
	i := Odd(10+1);			{ result: 1			}

	r := Sin(-3*pi/4);		{ result: -0.707107	}
	r := Sqr(10);			{ result: 100		}
	r := Sqrt(2);			{ result: 1.41421	}

	Writeln(i, r, pi);
{	Writeln(pi : 7 : 5);	  field specifiers not implemented... yet }
	Writeln
end .

