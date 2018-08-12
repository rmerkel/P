{ Simple for loop test	}
program ForTest() is
type R is 0..9;
var i : R;
begin
	put_line("for i in 0..9...");
	for i in 0..9 loop
		put_line(i)
	endloop;

	put_line("for i in R...");
	for i in R loop
		put_line(i)
	endloop
endprog
