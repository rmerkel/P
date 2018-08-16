program AttrTest() is
type
	R is 0..9;
	A is array [ R ] of character;
	E is ( X, Y, Z );
begin
	put("integer`min: ");
	put_line(integer`min);
	put("integer`max: ");
	put_line(integer`max);

	put("R`min: ");
	put_line(R`min);
	put("R`max: ");
	put_line(R`max);

	put("A`min: ");
	put_line(A`min);
	put("A`max: ");
	put_line(A`max);

	put("E`min: ");
	put_line(E`min);
	put("E`max: ");
	put_line(E`max)

endprog

