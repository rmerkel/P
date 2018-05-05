program PointerTest() is
var
	xp : ^integer;

begin
	xp := nil;
	writeln(xp);

	new(xp);
	writeln(xp);
	if xp <> nil then
		xp^ := 2048;
		writeln(xp^)
	endif;

	xp^ := 0;
	dispose(xp)
endprog
