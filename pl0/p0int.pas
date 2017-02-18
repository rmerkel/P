const
	amax=2047;      {maximum address}
	levmax=3;       {maximum depth of block nesting}
	cxmax=200;      {size of code array}

type 
	fct=(lit,opr,lod,sto,cal,int,jmp,jpc);
	instruction=packed record 
		f:fct;
		l:0..levmax;
		a:0..amax;
	end;

var
	code: array [0..cxmax] of instruction;

procedure interpret;

  const stacksize = 500;

  var
    p, b, t: integer; {program-, base-, topstack-registers}
    i: instruction; {instruction register}
    s: array [1..stacksize] of integer; {datastore}

  function base(l: integer): integer;
    var b1: integer;
  begin
    b1 := b; {find base l levels down}
    while l > 0 do begin
      b1 := s[b1];
      l := l - 1
    end;
    base := b1
  end {base};

begin
  writeln(' start pl/0');
  t := 0; b := 1; p := 0;
  s[1] := 0; s[2] := 0; s[3] := 0;
  repeat
    i := code[p]; p := p + 1;
    with i do
      case f of
        lit: begin t := t + 1; s[t] := a end;
        opr: 
          case a of {operator}
            0: 
              begin {return}
                t := b - 1; p := s[t + 3]; b := s[t + 2];
              end;
            1: s[t] := -s[t];
            2: begin t := t - 1; s[t] := s[t] + s[t + 1] end;
            3: begin t := t - 1; s[t] := s[t] - s[t + 1] end;
            4: begin t := t - 1; s[t] := s[t] * s[t + 1] end;
            5: begin t := t - 1; s[t] := s[t] div s[t + 1] end;
            6: s[t] := ord(odd(s[t]));
            8: begin t := t - 1; s[t] := ord(s[t] = s[t + 1]) end;
            9: begin t := t - 1; s[t] := ord(s[t] <> s[t + 1]) end;
            10: begin t := t - 1; s[t] := ord(s[t] < s[t + 1]) end;
            11: begin t := t - 1; s[t] := ord(s[t] >= s[t + 1]) end;
            12: begin t := t - 1; s[t] := ord(s[t] > s[t + 1]) end;
            13: begin t := t - 1; s[t] := ord(s[t] <= s[t + 1]) end;
          end;
        lod: begin t := t + 1; s[t] := s[base(l) + a] end;
        sto: begin s[base(l)+a] := s[t]; writeln(s[t]); t := t - 1 end;
        cal: 
          begin {generate new block mark}
            s[t + 1] := base(l); s[t + 2] := b; s[t + 3] := p;
            b := t + 1; p := a
          end;
        int: t := t + a;
        jmp: p := a;
        jpc: begin if s[t] = 0 then p := a; t := t - 1 end
      end {with, case}
  until p = 0;
  writeln(' end pl/0');
end {interpret};
