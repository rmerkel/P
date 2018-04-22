{ test builtin functions and procedures	}
program builtins() is
const
	pi = 3.1415926;

type
	e is (zero, one, two, three);

begin
	put_line(1);				{ 1			}
	put_line(round(2.0));		{ 2			}
	put_line(round(2.5));		{ 3 		}
	put_line(trunc(4.0));		{ 4 		}
	put_line(trunc(5.9));		{ 5 		}
	put_line(maxint);			{ 2,147,483,647	}
	
	put_line(abs(1));			{ 1			}
	put_line(abs(-1));			{ 1			}
	put_line(abs(1.5));			{ 1.5		}
	put_line(abs(-1.5));		{ 1.5		}

	put_line(ord(three));		{ 3			}
	put_line(ord(3-1));			{ 2			}

{	put_line(ord(1.2));		error: ordinal value expected	}

	put_line(arctan(1));		{ 0.785398	}
	put_line(arctan(1.0));		{ 0.785398	}

	put_line(exp(1));			{ 2.718282	}
	put_line(exp(1.0));			{ 2.718282	}

	put_line(ln(1));			{ 0.0		}
	put_line(ln(1.0));			{ 0.0		}

	put_line(odd(10));			{ false		}
	put_line(odd(10+1));		{ true		}

	put_line(sin(-3*pi/4));		{ -0.707107	}
	put_line(sqr(10));			{ 100		}
	put_line(sqrt(2));			{ 1.41421	}

	put(100);
	put(1.41421);
	put_line(pi);

	put_line(pi, 7, 5)
endprog

