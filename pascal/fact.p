var f, n;
procedure fact;
    var ans1;
    begin
        ans1 := n;
        n := n - 1;
        if n < 0 then f := -1;
        if n = 0 then f := 1;
        call fact;
        f := f * ans1;
    end;

begin
    n :=16;
    call fact;
end.

