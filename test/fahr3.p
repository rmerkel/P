{ print Fahrenheit-Celsius table	}
{	third version; integers & reals	}

program fahr3() is
const
	LOWER =   0.0;	{	lower table limit	}
	UPPER = 300.0;	{	upper table limit	}
	STEP  =  20.0;	{	table step size		}

var
	fahr : real ; celsius : integer;

begin
	putln("Fahrenheit Celsius");

	fahr := LOWER;
	while fahr <= UPPER loop
		celsius := round(5.0 * (fahr-32.0) / 9.0);
		put(fahr, 8, 1);
		putln(celsius, 9, 1);
		fahr := fahr + STEP;
	endloop
endprog
