# LAIN-OS

> "No matter where you are, everyone is always connected."

<p align="center">
  <img src="https://media.tenor.com/6R6W1n-LmNMAAAAi/lain-lain-iwakura.gif" alt="lain" width="200"/>
</p>

LainOS (Abyss) is an experimental RISC-V kernel written in C. It features a temporal fracture scheduler and a predictive shell with Markov chain-based command prediction.

## Features

- **Temporal Fracture Scheduler**: Process scheduling based on timeline priorities
- **Predictive Shell**: Tab completion using Markov chains that learn from your commands
- **Memory Management**: Physical and virtual memory management with paging
- **Process Management**: Full process lifecycle with fork/exec/exit
- **Virtual File System**: In-memory filesystem with basic operations
- **System Calls**: Standard Unix-like system call interface

## Building

Requirements:
- riscv64-unknown-elf-gcc
- riscv64-unknown-elf-binutils
- qemu-system-riscv64

```bash
make clean
make
```

## Running

```bash
make run
```

To exit QEMU, press `Ctrl+A` then `X`.

## Debugging

```bash
make debug
```

In another terminal:
```bash
riscv64-unknown-elf-gdb build/bin/kernel.elf
(gdb) target remote :1234
(gdb) continue
```

## Shell Commands

- `help` - Show available commands
- `neofetch` - Display system information  
- `ps` - List running processes
- `kill <pid>` - Terminate a process
- `mem` - Show memory statistics
- `clear` - Clear the screen
- `echo <text>` - Print text
- `ascend` - Trigger system state dump

## Architecture

```mermaid
graph TD
    subgraph Hardware
        QEMU[QEMU RISC-V]
    end

    subgraph AbyssCore
        START[start.S] --> KMAIN[kmain.c]
        KMAIN --> HEAP[Heap Allocator]
        KMAIN --> PAGING[Page Tables]
    end

    subgraph Subsystems
        KMAIN --> SCHEDULER[Temporal Scheduler]
        KMAIN --> VFS[NeuroVFS]
        KMAIN --> SHELL[Predictive Shell]
        KMAIN --> EGO[Ego Containers]
    end

    subgraph Tools
        SHELL --> DECOMPILER[Live Disassembler]
        SHELL --> PATCHER[Hot Patcher]
        SHELL --> ASCENSION[Ascension Trigger]
    end

    subgraph Drivers
        KMAIN --> QUANTUM[Quantum Driver]
        QEMU --> UART[UART 0x10000000]
    end

    SCHEDULER --> |timelines| PROCESSES[Processes]
    VFS --> |files| FILESYSTEM[/home/lain]
    SHELL --> |input| UART
    UART --> |output| SHELL
```

```
abyss/
├── arch/riscv64/     RISC-V specific code
├── kernel/           Core kernel (processes, scheduler, syscalls)
├── mm/               Memory management
├── fs/               Filesystem
├── drivers/          Device drivers
├── shell/            Shell and Markov predictor
├── lib/              Kernel library functions
└── init/             Kernel initialization
```

## Present Day. Present Time.

**HA-HA-HA-HA-HA.**
