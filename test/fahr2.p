{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

program fact2 ;
const
	LOWER =   0.0;	{	lower table limit	}
	UPPER = 300.0;	{	upper table limit	}
	STEP  =  20.0;	{	table step size		}

var
	fahr, celsius : Real;

begin
	fahr := LOWER;
	while fahr <= UPPER do begin
		celsius := 5.0 * (fahr-32.0) / 9.0;
		fahr := fahr + STEP;
	end;
 end.
