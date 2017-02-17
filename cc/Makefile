################################################################################
# @file makefile
#
# @brief makefile for pl0com (p/l 0 complier/interperter) 
################################################################################

################################################################################
# Build a debug (DEBUG=1: default) or release (DEBUG=0) binary?
################################################################################

# Support C++11, enable all, extra warnings, and generate dependency files
CXXFLAGS +=-std=c++11 -Wall -Wextra -MMD -MP

# Build for debugging by default, or release/optimized
DEBUG	?= 1
ifeq	($(DEBUG),1)
	CXXFLAGS += -O0 -g3 -DDEBUG
else
	CXXFLAGS += -O3 -DNDEBUG
endif

################################################################################
# Project files
################################################################################

COM_SRCS	= driver.cc pl0.cc pl0com.cc pl0int.cc symbol.cc token.cc
INT_SRCS	= pl0.cc pl0int.cc xpl0int.cc
SRCS		= driver.cc pl0.cc pl0int.cc pl0com.cc token.cc xpl0int.cc

COM_OBJS	= $(COM_SRCS:.cc=.o)
INT_OBJS	= $(INT_SRCS:.cc=.o)
OBJS		= $(COM_OBJS) $(INT_OBJS)

DEPS		= $(SRCS:.cc=.d)
EXES		= pl0com xpl0int

.PHONY:	all clean cleanall docs help pr test

################################################################################
#	The default target...
################################################################################

all:	$(EXES) docs

################################################################################
# pl0com
################################################################################

pl0com: $(COM_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(COM_OBJS)

################################################################################
# xpl0int
################################################################################

xpl0int: $(INT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(INT_OBJS)

################################################################################
# Include generated dependencies
################################################################################

-include $(DEPS)

################################################################################
# Cleanup intermediates...
################################################################################

clean:
	@rm -f $(OBJS) $(DEPS)

################################################################################
# Cleanup all targets and intermediates...
################################################################################

cleanall: clean
	@rm -rf $(EXES) docs

################################################################################
# Generate documentation
################################################################################

docs:	docs/html/index.html

docs/html/index.html:	Doxyfile $(SRCS)
	@doxygen

################################################################################
# Print a help message...
################################################################################

help:
	@echo "Usage:  make [DEBUG=0|1] targets..."
	@echo ""
	@echo "DEBUG=1, (default), builds a debug image and 0 builds a release."
	@echo ""
	@echo "Targets:"
	@echo "    all     - to build xpl0int and generate documentation (default)."
	@echo "    pl0com  - to build the compiler."
	@echo "    xpl0int - to build the test interupter."
	@echo "    clean   - to delete intermediates."
	@echo "    cleanll - to delete all targets and intermediates."
	@echo "    docs    - to generate documentation."
	@echo "    help    - prints this message."
	@echo "    pr      - print source"
	@echo "    test    - to bring calc upto date and run tests."
	@echo ""

################################################################################
# Print
################################################################################

pr:
	@pr $(SRCS) $(wildcard *.h) makefile | expand -4 | lpr

################################################################################
# Bring up to date and run some tests...
################################################################################

test: all
	./xpl0int
	./pl0com test.p
	./pl0com fact.p

