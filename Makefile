################################################################################
# @file makefile
#
# @brief makefile for Pascal-Lite complier & interperter
################################################################################

################################################################################
# Build a debug (DEBUG=1: default) or release (DEBUG=0) binary?
#
# Switch between GCC and CLang with
#   sudo update-alternatives --config c++
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

SRCS	= datum.cc driver.cc instr.cc comp.cc interp.cc symbol.cc token.cc type.cc
ALLSRCS	= $(SRCS) $(wildcard *.h)
OBJS	= $(SRCS:.cc=.o)
DEPS	= $(SRCS:.cc=.d)
EXE		= pas

LSTINGS = $(wildcard *p.lst)

.PHONY:	all clean cleanall docs help pr test

################################################################################
#	The default target...
################################################################################

all:	$(EXE) docs

################################################################################
# pas
################################################################################

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

################################################################################
# Include generated dependencies
################################################################################

-include $(DEPS)

################################################################################
# Cleanup intermediates...
################################################################################

clean:
	@rm -f $(OBJS) $(DEPS) $(LSTINGS)

################################################################################
# Cleanup all targets and intermediates...
################################################################################

cleanall: clean
	@rm -rf $(EXE) docs

################################################################################
# Generate documentation
################################################################################

docs:	docs/html/index.html

docs/html/index.html:	Doxyfile $(ALLSRCS)
	doxygen

################################################################################
# Print a help message...
################################################################################

help:
	@echo "Usage:  make [DEBUG=0|1] targets..."
	@echo ""
	@echo "DEBUG=1, (default), builds a debug image and 0 builds a release."
	@echo ""
	@echo "Targets:"
	@echo "    all     - to build the compilier  and generate documentation (default)."
	@echo "    clean   - to delete intermediates."
	@echo "    cleanll - to delete all targets and intermediates."
	@echo "    docs    - to generate documentation."
	@echo "    help    - prints this message."
	@echo "    pas     - to build the compiler."
	@echo "    pr      - prepare source for printing"
	@echo "    test    - to bring calc upto date and run tests."
	@echo ""

################################################################################
# Print
################################################################################

pr:
	@pr --expand-tabs=4 $(ALLSRCS) Makefile

################################################################################
# Bring up to date and run some tests...
################################################################################

test: all
	./xpas.sh

