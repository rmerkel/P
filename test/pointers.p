program PointerTest ;
var
	xp : ^integer;

begin
	xp := nil;
	new(xp);
	if xp = nil then
		writeln(xp);
	xp^ := 0;
	dispose(xp)
end .
