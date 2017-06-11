const max = 10;
var v[max], i, x;
begin 
	i = 0;
	while i < max do begin
		v[i] = i;
		i = i + 1
	end;

	i = 0;
	repeat begin
		x = v[i];
		i = i + 1
	end until i > max
end .
