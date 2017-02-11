var i;

    procedure a;
    var j;
        procedure aa;
        var jj;
        begin
            jj := i;
            i := i * 3;
            j := j * 3;
        end;
    begin
        j := i;
        i := i + 1;
        call aa;
    end;

    procedure b;
    var k;
        procedure bb;
        var kk;
            procedure bbb;
            var kkk;
            begin
                kkk := i;
                kk := kk - 3;
                k := k - 3;
                i := i - 3;
            end;
        begin
            kk := i;
            k := k - 2;
            i := i - 2;
            call bbb;
        end;
    begin
        k := i;
        i := i - 1;
        call bb;
        call a;
    end;

begin
    i := 0;
    call a;
    call b;
end.

