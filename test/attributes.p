program AttrTest() is
type
	R is 0..9;
	A is array [ R ] of character;
	E is ( X, Y, Z );
begin
	put("integer`min: ");
	putln(integer`min);
	put("integer`max: ");
	putln(integer`max);

	put("R`min: ");
	putln(R`min);
	put("R`max: ");
	putln(R`max);

	put("A`min: ");
	putln(A`min);
	put("A`max: ");
	putln(A`max);

	put("E`min: ");
	putln(E`min);
	put("E`max: ");
	putln(E`max)

endprog

