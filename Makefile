################################################################################
# @file makefile
#
# @brief makefile for P language complier & interperter
################################################################################

################################################################################
# Build a debug (DEBUG=1: default) or release (DEBUG=0) binary?
#
# Switch between GCC and CLang with
#   sudo update-alternatives --config c++
################################################################################

CXX = c++

# Support C++11, enable all, extra warnings, and generate dependency files
CXXFLAGS +=-std=c++11 -Wall -Wextra -MMD -MP

# Build for debugging (default), or release/optimized
DEBUG	?= 1
ifeq	($(DEBUG),1)
	CXXFLAGS += -O0 -g3 -DDEBUG
else
	CXXFLAGS += -O3 -DNDEBUG
endif

################################################################################
# Project directories
################################################################################

DOCDIR	= docs
OBJDIR	= objs

################################################################################
# Project files
################################################################################

SRCS	= $(wildcard *.cc)
ALLSRCS	= $(SRCS) $(wildcard *.h)
OBJS	= $(addprefix $(OBJDIR)/,$(SRCS:.cc=.o))
DEPS	= $(addprefix $(OBJDIR)/,$(SRCS:.cc=.d))
EXE		= p

LSTINGS = $(wildcard *p.lst)

################################################################################
# Rules
################################################################################

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


.PHONY:	all clean cleanall $(DOCDIR) help pr test

################################################################################
#	The default target...
################################################################################

all:	$(EXE) $(DOCDIR)

################################################################################
# p
################################################################################

$(EXE): $(OBJDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJDIR):
	@mkdir -p $(OBJDIR) 

################################################################################
# Include generated dependencies
################################################################################

-include $(DEPS)

################################################################################
# Cleanup intermediates...
################################################################################

clean:
	@rm -f $(LSTINGS) $(OBJDIR)/*

################################################################################
# Cleanup all targets and intermediates...
################################################################################

cleanall: clean
	@rm -rf $(EXE) $(DOCDIR) $(OBJDIR)

################################################################################
# Generate documentation
################################################################################

$(DOCDIR):	$(DOCDIR)/html/index.html

$(DOCDIR)/html/index.html:	Doxyfile $(ALLSRCS) README.md
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
	@echo "    check   - to run static checker."
	@echo "    clean   - to delete intermediates."
	@echo "    cleanll - to delete all targets and intermediates."
	@echo "    docs    - to generate documentation."
	@echo "    help    - prints this message."
	@echo "    p       - to build the compiler."
	@echo "    pr      - prepare source for printing"
	@echo "    test    - to bring calc upto date and run tests."
	@echo ""

################################################################################
# Print
################################################################################

pr:
	@pr --expand-tabs=4 $(ALLSRCS) Makefile README.md

################################################################################
		assert(false);
# Bring up to date and run some tests...
################################################################################

test: all
	./xp.sh
	./xp2.sh

