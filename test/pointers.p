program PointerTest() is
var
	xp : ^integer;

begin
	xp := nil;
	put_line(xp);

	new(xp);
	put_line(xp);
	if xp <> nil then
		xp^ := 2048;
		put_line(xp^)
	endif;

	xp^ := 0;
	dispose(xp)
endprog
