{ test builtin functions and procedures	}
program builtins() is
const
	pi = 3.1415926;

type
	e is (zero, one, two, three);

begin
	putln(1);					{ 1			}
	putln(round(2.0));			{ 2			}
	putln(round(2.5));			{ 3 		}
	putln(trunc(4.0));			{ 4 		}
	putln(trunc(5.9));			{ 5 		}
	putln(maxint);				{ 2,147,483,647	}
	
	putln(abs(1));				{ 1			}
	putln(abs(-1));				{ 1			}
	putln(abs(1.5));			{ 1.5		}
	putln(abs(-1.5));			{ 1.5		}

	putln(ord(three));			{ 3			}
	putln(ord(3-1));			{ 2			}

{	putln(ord(1.2));		error: ordinal value expected	}

	putln(arctan(1));			{ 0.785398	}
	putln(arctan(1.0));			{ 0.785398	}

	putln(exp(1));				{ 2.718282	}
	putln(exp(1.0));			{ 2.718282	}

	putln(ln(1));				{ 0.0		}
	putln(ln(1.0));				{ 0.0		}

	putln(odd(10));				{ false		}
	putln(odd(10+1));			{ true		}

	putln(sin(-3*pi/4));		{ -0.707107	}
	putln(sqr(10));				{ 100		}
	putln(sqrt(2));				{ 1.41421	}

	put(100);
	put(1.41421);
	putln(pi);

	putln(pi, 7, 5)
endprog

