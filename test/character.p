program CharTest ;
type
	A = array [0..9] of char;

var i : integer;
	c : char;
 	a1 : A;
 	a2 : A;

	procedure print(a : A) ;
	var i : integer;
	begin
{ 		writeln(a);			writeln doesn't support arrays... yet!	}

		for i := 0 to 9 do
			write(a[i]);
		writeln;
	end ;

begin
	c := 'x';
	writeln(c);	

	a1 := 'abcdefghij';		{	fill a1 with 'abcd...'			}
{ 	writeln(a1);				writeln doesn't support arrays... yet!	}
{	for i := 0 to 9 do
		write(a1[i]);
	writeln;													}
	print(a1);


	a2 := a1;				{	copies the contents of a1 to a2	}
{	writeln(a2);				writeln doesn't support arrays... yet!	}
	for i := 0 to 9 do
		write(a2[i]);
	writeln;

	a1 := '0123456788';		{	fill a1 with '0123...'			}
							{	while a1 has changed...			}
{	writeln(a1);				writeln doesn't support arrays... yet!	}
	for i := 0 to 9 do
		write(a1[i]);
	writeln;

							{	... a2 has not!					}
{	writeln(a2)					writeln doesn't support arrays!	}
	for i := 0 to 9 do
		write(a2[i]);
	writeln;
end .
