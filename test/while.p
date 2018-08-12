{ Simple while loop test }
program ForTest() is
type R is 0..9;
var i : R;
begin
	put_line("i := 0, i <= 9, i := i + 1");
	i := 0;
	while (i < 9) loop
		put_line(i);
		i := i + 1
	endloop;
	put_line(i);
	put_line();

	put_line("natual_min, 9, i := succ(i)");
	i := natural_min;
	while (i < 9) loop
		put_line(i);
		i := succ(i)
	endloop;
	put_line(i);
	put_line();

	put_line("R_min, R_max, i := succ(i)");
	i := R_min;
	while (i < R_max) loop
		put_line(i);
		i := succ(i)
	endloop;
	put_line(i)
endprog
