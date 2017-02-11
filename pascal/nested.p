    procedure a;
        procedure aa;
            procedure aaa;
            begin
            end;

        begin
            call aaa;
        end;

        procedure bb;
        begin
            call aa;
        end;

    begin
        call aa;
        call bb;
    end;

    procedure b;
    begin
        call a;
    end;

begin
    call a;
    call b;
end.
