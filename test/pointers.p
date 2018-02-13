program PointerTest ;
var
	xp : ^integer;

begin
	xp := nil;
	writeln(xp);

	new(xp);
	writeln(xp);
	if xp <> nil then begin
		xp^ := 2048;
		writeln(xp^)
	end;

	xp^ := 0;
	dispose(xp)
end .
