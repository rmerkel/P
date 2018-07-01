{ Simple while loop test }
program ForTest() is
type R is 0..9;
var i : R;
begin
	putln("i := 0, i <= 9, i := i + 1");
	i := 0;
	while (i < 9) loop
		putln(i);
		i := i + 1
	endloop;
	putln(i);

	putln("first(i), last(i), succ(i)");
	i := first(i);
	while (i < last(i)) loop
		putln(i);
		i := succ(i);
	endloop;
	putln(i)

endprog
