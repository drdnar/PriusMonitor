
PROGRAM_NAME = primon


SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

LIBS := -lpthread -lncurses

# define some common makefile things
empty :=
space := $(empty) $(empty)
comma := ,

# Verbosity
#Q = @
Q = 

# Compile options
ifeq ($(DEBUG_MODE),NO_DEBUG)
OPTIMIZATION = -O0 -fstandalone-debug -g -fdebug-macro -Wdocumentation
else
OPTIMIZATION = -O3 -g0
endif
LINKER = llvm-ld
CXX := clang++
CXXFLAGS := -c -Wall -Wextra $(OPTIMIZATION)

# Some OS-specific stuff
NATIVEPATH = $(subst \,/,$1)
RM = rm -f $1
RMDIR = rm -rf $1
NATIVEMKDR = mkdir -p $1
QUOTE_ARG = '$(subst ','\'',$1)'#'

# source: http://blog.jgc.org/2011/07/gnu-make-recursive-wildcard-function.html
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

# find source files
CPPSOURCES := $(call rwildcard,$(SRCDIR),*.cpp)
USERHEADERS := $(call rwildcard,$(SRCDIR),*.h *.hpp)

# create links for later
CPP_OBJECTS := $(CPPSOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)


all: $(BINDIR)/$(PROGRAM_NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(USERHEADERS)
	$(Q)mkdir -p obj/UI/forms obj/Vehicle bin
	$(Q)$(CXX) $(CXXFLAGS) $(call QUOTE_ARG,$(addprefix $(CURDIR)/,$<)) -o $(call QUOTE_ARG,$(addprefix $(CURDIR)/,$@))

$(BINDIR)/$(PROGRAM_NAME): $(CPP_OBJECTS)
	$(Q)$(CXX) $(CPP_OBJECTS) $(LIBS) -o $(BINDIR)/$(PROGRAM_NAME)

clean:
	$(Q)rm -r bin obj

.PHONY: clean all
