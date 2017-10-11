{ test builtin functions and procedures	}
program builtins ;
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
	i := IntMax;			{ result 2**32		}
	
	i := Abs(1);			{ result: 1			}
	i := Abs(-1);			{ result: 1			}
	r := Abs(1.5);			{ result 1.5		}
	r := Abs(-1.5);			{ result 1.5		}

	i := Ord(three);		{ result: 3			}
	i := Ord(i-1);			{ result: 2			}

{	i := Ord(1.2)			error: ordinal value expected	}

	r := Atan(1);			{ result:  0.785398	}
	r := Atan(1.0);			{ result:  0.785398	}

	r := Exp(1);			{ result: 2.718282	}
	r := Exp(1.0);			{ result: 2.718282	}

	r := Log(1);			{ result: 0.0		}
	r := Log(1.0);			{ result: 0.0		}

	i := Odd(10);			{ result: 0			}
	i := Odd(10+1)			{ result: 1			}
end .
