CPROGS = $(shell find . -name '*\.c' -not -path './lmode/*')
COUTPUT = $(CPROGS:.c=.o)
ASMPROGS = $(shell find . -name '*\.asm' -not -path './lmode/*')
ASMOUTPUT  =$(ASMPROGS:.asm=.o)

arch_build: $(COUTPUT) $(ASMOUTPUT)

%.o: %.c
	@-mkdir -p $(dir $(PREFIX)/$@)
	@$(CC) $(CFLAGS) -c $< -o $(PREFIX)/$@

%.o: %.asm
	@-mkdir -p $(dir $(PREFIX)/$@)
	@nasm $(NASMFLAGS) -o $(PREFIX)/$@ $<
