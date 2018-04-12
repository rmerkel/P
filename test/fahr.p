{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

program fahrtest()
const
	LOWER =   0;	{	lower table limit	}
	UPPER = 300;	{	upper table limit	}
	STEP  =  20;	{	table step size		}

var
	fahr, celsius : real;

begin
	writeln("Fahrenheit Celsius");

	fahr := LOWER;
	while fahr <= UPPER loop
		celsius := 5.0 * (fahr-32.0) / 9.0;
		writeln(fahr : 9 : 1, celsius : 8 : 1);
		fahr := fahr + STEP;
	endloop
 end ;
