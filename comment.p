{ "main" starts here... }
var n, f;	{ var z; parser doesn't see 'z' }
begin
   n = 0;
   f = 1;
   {	calculate factor (n)
		comment continued on this line... }
   while n < 10 do begin
      n = n + 1;
      f = f * n
   end
end.

{	unterminated comment, but we don't care as it follows the period!
