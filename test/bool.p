program BoolTest()
var
	b : boolean;
begin
	b := true;
	if (b) then b := false endif;
	writeln(b)
end;
