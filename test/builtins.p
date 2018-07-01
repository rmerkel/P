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

	putln(true);				{ true				}
	putln(false);				{ false				}
	putln(character_min);		{ 0					}
 	putln(character_max);		{ 127				}
	putln(integer_min);			{ -2,147,483,648	}
	putln(integer_max);			{ 2,147,483,647		}
	putln(natural_min);			{ 0					}
	putln(natural_max);			{ 2,147,483,647		}
	putln(positive_min);		{ 1					}
	putln(positive_max);		{ 2,147,483,647		}
	putln(real_min);			{ 2.225074e-308		}
	putln(real_max);			{ 1.797693e+308		}
	
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

