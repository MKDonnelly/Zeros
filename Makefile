VERSION := 0.4

ARCH ?= x64


define make-build
   for dir in $(src_dirs); \
   do  \
      mkdir -p $(objdir)/$$dir; \
   done
endef

include arch/rules-$(ARCH).mk

$(objdir)/%.o: %.c
	@echo "   CC    $<"
	@$(CC) -c $(CFLAGS) $< -o $@

$(objdir)/%.o: %.asm
	@echo "   ASM   $<"
	@$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: pre-build all post-build clean run qemu qemu_gdb 
