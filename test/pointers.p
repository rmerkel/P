program PointerTest() is
var
	xp : ^integer;

begin
	xp := nil;
	putln(xp);

	new(xp);
	putln(xp);
	if xp <> nil then
		xp^ := 2048;
		putln(xp^)
	endif;

	xp^ := 0;
	dispose(xp)
endprog
