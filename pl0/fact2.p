var p, n;
    procedure factorial;
    begin
        p := 1;
        while n > 1 do begin
            p := p * n;
            n := n - 1
        end
    end;

begin
    n := 10;
    call factorial
end.
