{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

const
	LOWER =   0.0;	{	lower table limit	}
	UPPER = 300.0;	{	upper table limit	}
	STEP  =  20.0;	{	table step size		}

var
	fahr : real ; celsius : integer;

begin
	fahr = LOWER;
	while fahr <= UPPER do begin
		celsius = round(5.0 * (fahr-32.0) / 9.0);
		fahr = fahr + STEP;
	end;
 end.
