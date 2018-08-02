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
	putln();

	putln("natural`min, 9, i := succ(i)");
	i := natural`min;
	while (i < 9) loop
		putln(i);
		i := succ(i)
	endloop;
	putln(i);
	putln();

	putln("R`min, R`max, i := succ(i)");
	i := R`min;
	while (i < R`max) loop
		putln(i);
		i := succ(i)
	endloop;
	putln(i)
endprog
