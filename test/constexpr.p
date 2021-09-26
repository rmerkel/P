program TestConstExpr() is 
const i = 0;
	  j = 1+2;
	  k = j*10/2;
	  l = k < 2;
	  m = k bit_sleft 2;
	  n = j*(10/2);
	  p = integer`max;

begin
	put("i = ");
	putln(i);

	put("j = ");
	putln(j);

	put("k = ");
	putln(k);

	put("l = ");
	putln(l); 

	put("m = ");
	putln(m);

	put("n = ");
	putln(n);

	put("p = ");
	putln(p)
endprog
