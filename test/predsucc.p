program PredSucc ;
type 
	e = (zero, one, two, three, four);

begin
	Writeln(zero);
	Writeln(Pred(two));
	Writeln(two);
	Writeln(Succ(two));
end .
