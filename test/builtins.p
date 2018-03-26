{ test builtin functions and procedures	}
program builtins()
const
	pi = 3.1415926;

type
	e = (zero, one, two, three);

begin
	writeln(1);				{ 1			}
	writeln(round(2.0));	{ 2			}
	writeln(round(2.5));	{ 3 		}
	writeln(trunc(4.0));	{ 4 		}
	writeln(trunc(5.9));	{ 5 		}
	writeln(maxint);		{ 2,147,483,647	}
	
	writeln(abs(1));		{ 1			}
	writeln(abs(-1));		{ 1			}
	writeln(abs(1.5));		{ 1.5		}
	writeln(abs(-1.5));		{ 1.5		}

	writeln(ord(three));	{ 3			}
	writeln(ord(3-1));		{ 2			}

{	writeln(ord(1.2));		error: ordinal value expected	}

	writeln(arctan(1));		{ 0.785398	}
	writeln(arctan(1.0));	{ 0.785398	}

	writeln(exp(1));		{ 2.718282	}
	writeln(exp(1.0));		{ 2.718282	}

	writeln(ln(1));			{ 0.0		}
	writeln(ln(1.0));		{ 0.0		}

	writeln(odd(10));		{ false		}
	writeln(odd(10+1));		{ true		}

	writeln(sin(-3*pi/4));	{ -0.707107	}
	writeln(sqr(10));		{ 100		}
	writeln(sqrt(2));		{ 1.41421	}

	writeln(100, 1.41421, pi);
	writeln(pi : 7 : 5);
	writeln
end .

