{ Simple for loop test	}
program ForTest() is
type R is 0..9;
var i : R;
begin
	put("first: ");
	putln(first(i));

	put("last: ");
	putln(last(i));

	putln("for i in 0..9...");
	for i in 0..9 loop
		putln(i)
	endloop;

	putln("for i in R...");
	for i in R loop
		putln(i)
	endloop
endprog
