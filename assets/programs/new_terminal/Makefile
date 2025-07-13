AS=nasm
CC=i686-elf-gcc
LD=i686-elf-ld
CFLAGS=-g -falign-jumps -falign-functions -falign-labels -falign-loops \
        -fstrength-reduce -fomit-frame-pointer -finline-functions \
        -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp \
        -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall \
        -O0 -std=gnu99

OBJDIR=build
SRCDIR=src

OBJS= \
	$(OBJDIR)/start.asm.o \
	$(OBJDIR)/new_terminal.o \
	$(OBJDIR)/vios.o \
	$(OBJDIR)/stdlib.o \
	$(OBJDIR)/stdio.o \
	$(OBJDIR)/string.o \
	$(OBJDIR)/memory.o \
	$(OBJDIR)/start.o

.PHONY: all clean

all: $(OBJS)
	@echo "Linking..."
	@$(LD) -m elf_i386 -relocatable $(OBJS) -o new_terminal.elf

$(OBJDIR)/start.asm.o: ../stdlib/src/start.asm
	@mkdir -p $(OBJDIR)
	@$(AS) -f elf $< -o $@

$(OBJDIR)/new_terminal.o: $(SRCDIR)/new_terminal.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

$(OBJDIR)/vios.o: ../stdlib/src/vios.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

$(OBJDIR)/stdlib.o: ../stdlib/src/stdlib.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

$(OBJDIR)/stdio.o: ../stdlib/src/stdio.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

$(OBJDIR)/string.o: ../stdlib/src/string.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

$(OBJDIR)/memory.o: ../stdlib/src/memory.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

$(OBJDIR)/start.o: ../stdlib/src/start.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -I$(SRCDIR) -I../stdlib/src -c $< -o $@

clean:
	@rm -rf $(OBJDIR) *.elf
