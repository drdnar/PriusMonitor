#
# Makefile
#
CC ?= clang
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare
BIN = liblvglfonts.a

OBJEXT ?= .o

CSRCS = B612Regular16.c B612Regular22.c B612Bold16.c B612Bold22.c B612Italic16.c B612Italic22.c B612BoldItalic16.c B612BoldItalic22.c B612MonoRegular16.c B612MonoRegular22.c B612MonoBold16.c B612MonoBold22.c B612MonoItalic16.c B612MonoItalic22.c B612MonoBoldItalic16.c B612MonoBoldItalic22.c
COBJS = $(CSRCS:.c=$(OBJEXT))

SRCS = $(CSRCS)
OBJS = $(COBJS)

## MAINOBJ -> OBJFILES

all: $(BIN)

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
$(BIN): $(AOBJS) $(COBJS)
	ar -rv $(BIN) $(OBJS)
#	$(CC) -o $(BIN) $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(COBJS)

.PHONY: clean
