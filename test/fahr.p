{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

program fahrtest() is
const
	LOWER =   0;	{	lower table limit	}
	UPPER = 300;	{	upper table limit	}
	STEP  =  20;	{	table step size		}

var
	fahr, celsius : real;

begin
	putln("Fahrenheit Celsius");

	fahr := LOWER;
	while fahr <= UPPER loop
		celsius := 5.0 * (fahr-32.0) / 9.0;
		put(fahr, 9, 1);
		putln(celsius, 8, 1);
		fahr := fahr + STEP;
	endloop
endprog
