{ print Fahrenheit-Celsius table	}
{	first version; integers only	}

const
	LOWER =   0;	{	lower table limit	}
	UPPER = 300;	{	upper table limit	}
	STEP  =  20;	{	table step size		}

var
	fahr, celsius : integer;

begin
	fahr = LOWER;
	while fahr <= UPPER do begin
		celsius = 5 * (fahr-32) / 9;
		fahr = fahr + STEP;
	end;
 end.
