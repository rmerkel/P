{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

program fahrtest ;
const
	LOWER =   0;	{	lower table limit	}
	UPPER = 300;	{	upper table limit	}
	STEP  =  20;	{	table step size		}

var
	fahr, celsius : real;

begin
	fahr := LOWER;
	while fahr <= UPPER do begin
		celsius := 5.0 * (fahr-32.0) / 9.0;
		fahr := fahr + STEP;
		writeln(fahr : 6 : 1, celsius : 6 : 1);
	end
 end.
