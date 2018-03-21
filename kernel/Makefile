CPROGS := $(shell find . -name '*\.c')
COUTPUT := $(CPROGS:.c=.o)

kernel_build: $(COUTPUT) 

%.o: %.c
	@-mkdir -p $(dir $(PREFIX)/$@)
	@$(CC) $(CFLAGS) -c $< -o $(PREFIX)/$@

clean:
	@find . -name '*\.o' -exec rm -Rf {} \;

