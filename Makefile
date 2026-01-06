ARCH ?= riscv64

ifeq ($(ARCH),riscv64)
    TRIPLE = riscv64-elf
    CXX = $(TRIPLE)-g++
    CC = $(TRIPLE)-gcc
    LD = $(TRIPLE)-ld
    QEMU = qemu-system-riscv64
    QEMU_FLAGS = -machine virt -bios default -kernel kernel.elf -nographic -serial mon:stdio
    CXXFLAGS_ARCH = -mcmodel=medany
else ifeq ($(ARCH),x86_64)
    TRIPLE = x86_64-elf
    CXX = $(TRIPLE)-g++
    CC = $(TRIPLE)-gcc
    LD = $(TRIPLE)-ld
    QEMU = qemu-system-x86_64
    QEMU_FLAGS = -kernel kernel.elf -nographic -serial mon:stdio
    CXXFLAGS_ARCH = -m64 -mno-red-zone
endif

CXXFLAGS = -std=c++20 -O2 -g -Wall -Wextra -Werror \
           -fno-exceptions -fno-rtti -nostdlib -ffreestanding \
           -fno-threadsafe-statics -fno-use-cxa-atexit \
           $(CXXFLAGS_ARCH) -Iabyss

LDFLAGS = -T linker_$(ARCH).ld -nostdlib -z max-page-size=4096

SRCS_ASM = abyss/start.S abyss/hyper.S
SRCS_CPP = abyss/kmain.cpp

OBJS = $(SRCS_ASM:.S=.o) $(SRCS_CPP:.cpp=.o)

all: kernel.elf

kernel.elf: $(OBJS) linker_$(ARCH).ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	$(CC) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

syntax:
	g++ -std=c++20 -fsyntax-only -Iabyss -D__riscv -nostdlib -ffreestanding $(SRCS_CPP)

run: kernel.elf
	$(QEMU) $(QEMU_FLAGS)

clean:
	rm -f $(OBJS) kernel.elf

.PHONY: all run clean
