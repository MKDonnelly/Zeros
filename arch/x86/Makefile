CPROGS = $(shell find . -name '*\.c' -not -path './pmode/*')
COUTPUT = $(CPROGS:.c=.o)
ASMPROGS = $(shell find . -name '*\.asm' -not -path './pmode/*')
ASMOUTPUT  =$(ASMPROGS:.asm=.o)

arch_build: $(COUTPUT) $(ASMOUTPUT)

%.o: %.c
	@-mkdir -p $(dir $(PREFIX)/$@)
	@$(CC) $(CFLAGS) -c $< -o $(PREFIX)/$@

%.o: %.asm
	@-mkdir -p $(dir $(PREFIX)/$@)
	@nasm $(NASMFLAGS) -o $(PREFIX)/$@ $<
