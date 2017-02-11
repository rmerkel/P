var x, n, f;
begin
   x := 2047;
   n := 0;
   f := 1;
   while n # 16 do
   begin
      n := n + 1;
      f := f * n;
   end;
end.

