# Makefile adapted from general-purpose Makefile by Job Vranish
# "Even simpler Makefile"
# https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

LIBDIR	?= lib
LIBRARY ?= $(LIBDIR)/zsound.lib

EXEC ?= YMTESTER.PRG
SRC_DIRS ?= ./src

CC		= cl65
AS		= cl65
LD		= cl65
AR		= ar65

FLAGS		= -t cx16 -g -Ln $(addsuffix .sym,$(basename $(EXEC)))
CFLAGS		= $(FLAGS) -O $(INC_FLAGS)
ASFLAGS		= $(FLAGS) -c
LDFLAGS		= $(FLAGS) -C zsound.cfg -u __EXEHDR__ -o

ASMSRCS := $(shell find $(SRC_DIRS) -name \*.asm)
ASMOBJS := $(addsuffix .o,$(basename $(ASMSRCS)))
CSRCS := $(shell find $(SRC_DIRS) -name \*.c)
COBJS := $(addsuffix .o,$(basename $(CSRCS)))
OBJS := $(COBJS) $(ASMOBJS)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

all:
	cl65 $(FLAGS) -o $(EXEC) $(ASMSRCS) $(CSRCS)

$(EXEC): $(OBJS)
	@echo $(ASMOBJS)

#src/%.o38: src/%.asm
#	$(AS) $(ASFLAGS) $(REV38) -o $@ $<
#
#src/%.o39: src/%.asm
#	$(AS) $(ASFLAGS) $(REV39) -o $@ $<
#
src/_%.o: src/%.asm
	$(AS) $(ASFLAGS) -o $@ $<

src/%.o: src/%.c


.PHONY: clean
clean:
	$(RM) $(EXEC) $(OBJS) $(DEPS) $(addsuffix .sym,$(basename $(EXEC)))

.PHONY: objclean
objclean:
	$(RM) $(OBJS)

.PHONY: test
test: $(EXEC)

.PHONY: %.h

.PHONY: %.inc

-include $(DEPS)
