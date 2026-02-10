# Lain-OS Installation & Deployment Guide

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf
sudo apt-get install qemu-system-misc
```

### Arch Linux
```bash
sudo pacman -S riscv64-elf-gcc riscv64-elf-binutils qemu-arch-extra
```

### macOS
```bash
brew tap riscv/riscv
brew install riscv-tools qemu
```

### Building from Source (if packages unavailable)
```bash
git clone https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv64gc --with-abi=lp64d
make
export PATH=/opt/riscv/bin:$PATH
```

## Quick Start

1. Clone or extract Lain-OS
```bash
cd lain-os-complete
```

2. Build
```bash
make clean
make
```

3. Run
```bash
make run
```

## Troubleshooting

### "riscv64-unknown-elf-gcc: command not found"
Solution: Install RISC-V toolchain or add it to PATH

### Build errors about missing headers
Solution: Check that all include paths are correct in Makefile

### QEMU hangs on boot
Solution: 
- Check that kernel.elf was created in build/bin/
- Try with more memory: modify Makefile to use -m 1024M

### No output in QEMU
Solution:
- Ensure -nographic flag is present
- Check UART initialization in uart.c

## Advanced Configuration

### Changing Memory Size
Edit Makefile, line with qemu-system-riscv64:
```makefile
-m 1024M    # Change from 512M to 1024M
```

### Enabling Debugging
```bash
make debug
```
Then in another terminal:
```bash
riscv64-unknown-elf-gdb build/bin/kernel.elf
(gdb) target remote :1234
(gdb) break kmain
(gdb) continue
```

### Build Optimizations
Edit Makefile CFLAGS:
```makefile
CFLAGS = -O3    # Change from -O2 for more optimization
```

## Project Structure

```
lain-os-complete/
├── abyss/                    Source code
│   ├── arch/riscv64/
│   │   ├── boot/            Boot assembly
│   │   ├── trap/            Trap handling  
│   │   ├── timer/           Timer driver
│   │   └── mm/              Memory management
│   ├── kernel/
│   │   ├── proc/            Process management
│   │   ├── sched/           Scheduler
│   │   └── syscall/         System calls
│   ├── mm/                  Memory allocators
│   ├── fs/vfs/              Virtual filesystem
│   ├── drivers/char/        Character devices
│   ├── lib/                 Kernel library
│   ├── shell/               Shell & Markov
│   └── init/                Kernel init
├── include/                 Header files
├── build/                   Build output
├── Makefile                 Build system
├── linker.ld               Linker script
└── README.md               Documentation
```

## Development Workflow

1. Make changes to source files
2. Run `make` to build
3. Run `make run` to test
4. Debug issues with `make debug` + GDB

## Common Modifications

### Adding a new system call
1. Add number to include/kernel/syscall.h
2. Add implementation to abyss/kernel/syscall/syscall.c
3. Add dispatch case in syscall_dispatch()

### Adding a shell command
1. Add cmd_yourcommand() function to abyss/shell/shell.c
2. Add command check in shell_execute()

### Adding a driver
1. Create driver in abyss/drivers/
2. Add header to include/drivers/
3. Initialize in kmain()

## Performance Tips

- Increase MARKOV_SIZE in markov.c for better predictions
- Tune TIMER_INTERVAL in timer.c for scheduling frequency
- Adjust timeline quantum in scheduler.c
- Increase NPROC for more processes

## License

Experimental kernel - use at your own risk.

Present Day. Present Time.
