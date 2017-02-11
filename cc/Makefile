CFLAGS	= -std=c++11 -Wall -Wextra -g
PFLAGS	= -l- -vi- -Tlinux -Miso -g

SRC		= p0int.cxx p0com.p

all: p0int p0com html/index.html

p0int:	p0int.cxx
	c++ $(CFLAGS) -o $@ $^

p0com: p0com.p
	fpc $(PFLAGS) $^

html/index.html:	$(SRC)
	@doxygen


