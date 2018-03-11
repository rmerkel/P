{    arrays test...                                }
program VarParamTest ;
type A9 = array [0..9] of integer;

var i  : integer;
    a1 : A9;
    a2 : A9;

    {    increment each element of a                }
    {    a must be a var to pass by reference    }
    procedure inc(var a : A9) ;
    var i : integer;
    begin
        for i := 0 to 9 do 
            a[i] := a[i] + 1;
    end;

begin
    for i := 0 to 9 do begin
        a1[i] := i;
        a2[i] := i + 10;
    end ;

    writeln(a1);
    writeln(a2);

    inc(a1);
    writeln(a1)
end .

