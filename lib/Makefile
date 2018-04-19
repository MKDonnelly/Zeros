CPROGS := $(shell find . -name '*\.c')
COUTPUT := $(CPROGS:.c=.o)

lib_build: $(COUTPUT) $(ASMOUTPUT)

%.o: %.c
	@-mkdir -p $(dir $(PREFIX)/$@)
	@$(CC) $(CFLAGS) -c $< -o $(PREFIX)/$@

clean:
	@find . -name '*\.o' -exec rm -Rf {} \;
