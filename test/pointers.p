program PointerTest ;
var
	xp : ^Integer;

begin
	xp := nil;
	New(xp);
	if xp = nil then
		Writeln(xp);
	xp^ := 0;
	Dispose(xp)
end .
