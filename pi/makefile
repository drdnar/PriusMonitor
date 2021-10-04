
PROGRAM_NAME = primon


SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

STATIC_LIBS_DIRS += -Llibs/
LIBS := -lpthread -lncurses -llvgl -llvglfonts

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
CC := clang
CXX := clang++
CXXFLAGS := -c -Wall -Wextra $(OPTIMIZATION)

# LVGL
LVGL_DIR ?= ${shell pwd}/libs
LVGL_DIR_NAME ?= lvgl

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
CSOURCES := $(call rwildcard,$(SRCDIR),*.c)
USERHEADERS := $(call rwildcard,$(SRCDIR),*.h *.hpp)

# create links for later
CPP_OBJECTS := $(CPPSOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
C_OBJECTS := $(CSOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)


all: staticlibs $(BINDIR)/$(PROGRAM_NAME)

staticlibs:
	$(MAKE) -C libs

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(USERHEADERS)
	$(Q)mkdir -p obj/UI/forms obj/Vehicle bin obj/UI/lvgl
	$(Q)$(CXX) $(CXXFLAGS) $(call QUOTE_ARG,$(addprefix $(CURDIR)/,$<)) -o $(call QUOTE_ARG,$(addprefix $(CURDIR)/,$@))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(USERHEADERS)
	$(Q)mkdir -p obj/UI/lvgl/core obj/Vehicle bin
	$(Q)$(CC) $(CXXFLAGS) $(call QUOTE_ARG,$(addprefix $(CURDIR)/,$<)) -o $(call QUOTE_ARG,$(addprefix $(CURDIR)/,$@))

$(BINDIR)/$(PROGRAM_NAME): $(CPP_OBJECTS) $(C_OBJECTS) staticlibs
	$(Q)$(CXX) $(CPP_OBJECTS) $(C_OBJECTS) $(STATIC_LIBS_DIRS) $(LIBS) -o $(BINDIR)/$(PROGRAM_NAME)

clean:
	$(Q)rm -r bin obj

veryclean: clean
	$(MAKE) clean -C libs

.PHONY: clean all staticlibs veryclean
