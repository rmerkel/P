{ print Fahrenheit-Celsius table	}
{	third version; integers & reals	}

program fahr3()
const
	LOWER =   0.0;	{	lower table limit	}
	UPPER = 300.0;	{	upper table limit	}
	STEP  =  20.0;	{	table step size		}

var
	fahr : real ; celsius : integer;

begin
	writeln("Fahrenheit Celsius");

	fahr := LOWER;
	while fahr <= UPPER loop
		celsius := round(5.0 * (fahr-32.0) / 9.0);
		writeln(fahr : 8 : 1, celsius : 9 : 1);
		fahr := fahr + STEP;
	endloop
 end ;
