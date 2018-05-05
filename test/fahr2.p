{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

program fact2() is
const
	LOWER =   0.0;	{	lower table limit	}
	UPPER = 300.0;	{	upper table limit	}
	STEP  =  20.0;	{	table step size		}

var
	fahr, celsius : real;

begin
	writeln("Fahrenheit Celsius");

	fahr := LOWER;
	while fahr <= UPPER loop
		celsius := 5.0 * (fahr-32.0) / 9.0;
		write(fahr, 9, 1);
		writeln(celsius, 8, 1);
		fahr := fahr + STEP;
	endloop
endprog
