CPROGS := $(shell find . -name '*\.c')
COUTPUT := $(CPROGS:.c=.o)

staging_build: $(COUTPUT) $(ASMOUTPUT)
	@echo $(ASMPROGS) $(ASMOUTPUT)

%.o: %.c
	@-mkdir -p $(dir $(PREFIX)/$@)
	@$(CC) $(CFLAGS) -c $< -o $(PREFIX)/$@


