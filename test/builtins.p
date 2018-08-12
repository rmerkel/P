{ test builtin functions and procedures	}
program builtins() is
const
	PI = 3.1415926;

type
	E is (zero, one, two, three);
	R is 0..127;

begin
	put_line(round(0.9));		{ 1			}
	put_line(round(2.0));		{ 2			}
	put_line(round(2.5));		{ 3 		}
	put_line(trunc(4.0));		{ 4 		}
	put_line(trunc(5.9));		{ 5 		}

	put_line(true);				{ true				}
	put_line(false);			{ false				}
	put_line(character_min);	{ 0					}
 	put_line(character_max);	{ 127				}
	put_line(integer_min);		{ -2,147,483,648	}
	put_line(integer_max);		{ 2,147,483,647		}
	put_line(natural_min);		{ 0					}
	put_line(natural_max);		{ 2,147,483,647		}
	put_line(positive_min);		{ 1					}
	put_line(positive_max);		{ 2,147,483,647		}
	put_line(real_min);			{ 2.225074e-308		}
	put_line(real_max);			{ 1.797693e+308		}
	
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

	put_line(sin(-3*PI/4));		{ -0.707107	}
	put_line(sqr(10));			{ 100		}
	put_line(sqrt(2));			{ 1.41421	}

	put(100);
	put(1.41421);
	put_line(PI);

	put_line(PI, 7, 5);

	put_line(E_min);
	put_line(E_max)
endprog

