CXXFLAGS	= -std=c++11 -Wall -Wextra
PFLAGS	= -l- -vi- -Tlinux -Miso -g

# Build for debugging by default, or release/optimized
DEBUG	?= 1
ifeq	($(DEBUG),1)
	CXXFLAGS += -O0 -g3 -DDEBUG
else
	CXXFLAGS += -O3 -DNDEBUG
endif

SRC	= pl0int.cxx pl0com.cxx p0com.p

all:	p0int pl0com p0com html/index.html

xpl0int.o:	pl0int.h
pl0int.o:	pl0int.h

xpl0int:	xpl0int.o pl0int.o pl0int.h
	c++ $(CXXFLAGS) -o $@ $^

pl0com: pl0com.cxx
	c++ $(CXXFLAGS) -o $@ $^

p0com: p0com.p
	fpc $(PFLAGS) $^

html/index.html:	$(SRC)
	@doxygen


