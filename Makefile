CROSS = riscv64-elf-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump

CFLAGS = -Wall -Werror -O2 -ffreestanding -nostdlib -std=gnu99 -std=gnu99
CFLAGS += -march=rv64gc -mabi=lp64d
CFLAGS += -mcmodel=medany
CFLAGS += -g -MD
CFLAGS += -Iinclude

LDFLAGS = -z max-page-size=4096

SRCDIR = abyss
OBJDIR = build
BINDIR = $(OBJDIR)/bin

SRCS_S = $(shell find $(SRCDIR) -name '*.S')
SRCS_C = $(shell find $(SRCDIR) -name '*.c')

OBJS = $(patsubst $(SRCDIR)/%.S,$(OBJDIR)/%.o,$(SRCS_S))
OBJS += $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS_C))

KERNEL = $(BINDIR)/kernel.elf

.PHONY: all clean run debug

all: $(KERNEL)

$(KERNEL): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $^
	$(OBJDUMP) -D $@ > $(BINDIR)/kernel.asm
	@echo "Build complete: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(KERNEL)
	qemu-system-riscv64 -machine virt -cpu rv64 -smp 1 \
		-m 512M -nographic \
		-bios none -kernel $(KERNEL)

debug: $(KERNEL)
	qemu-system-riscv64 -machine virt -cpu rv64 -smp 1 \
		-m 512M -nographic \
		-bios none -kernel $(KERNEL) \
		-s -S

clean:
	rm -rf $(OBJDIR)

-include $(OBJS:.o=.d)
