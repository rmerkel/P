program divbyzero ;
var x, y, z : Integer;

begin
	y := 10;
	z := 0;

	x := y / 2;
	x := y / z	{	opps!	}
end.

