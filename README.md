# LainOS/Abyss

## Prerequisites
To build and run LainOS, you need the RISC-V GNU Toolchain and QEMU.

### Debian/Ubuntu
```bash
sudo apt install gcc-riscv64-unknown-elf qemu-system-riscv64
```

### Arch Linux
```bash
sudo pacman -S riscv64-elf-gcc qemu-system-riscv
```

### macOS
```bash
brew tap riscv-software-src/riscv
brew install riscv-tools qemu
```

## Building
To build the kernel image (`kernel.elf`):
```bash
make
```

To verify syntax (if you don't have the cross-compiler):
```bash
make syntax
```

## Running
To run the kernel in QEMU:
```bash
make run
```

To exit QEMU, press `Ctrl+A` then `X`.

## Architecture Overview

LainOS (codenamed Abyss) is an experimental, production-grade kernel designed to challenge traditional OS concepts. It is built with the following philosophy: **No fluff, just raw, advanced systems.**

### Layer 0: AbyssCore (Minimal C++20 Kernel Base)
- **Entry**: `_start` in `start.S` (pure assembly) sets up the stack and jumps to `kmain.cpp`.
- **Runtime**: Custom STL-lite (`vector`, `unique_ptr`, `span`, `string_view`, `optional`) to avoid all external dependencies.
- **Memory**: Basic bump allocator for `new`/`delete` support.
- **Panic**: DWARF-based unwinding (stubbed) for detailed crash reports.

### Layer 1: Temporal Fracture Scheduler
- **Concept**: Instead of simple time-slicing, processes have "Timelines".
- **Mechanism**: `timer_interrupt()` saves state to the current timeline. `syscall_yield()` branches reality by copying the current timeline.
- **Scheduling**: The scheduler picks the "highest probability" timeline based on entropy and system state (e.g., CPU temperature).

### Layer 2: NeuroVFS (Per-User Content Mutation Filesystem)
- **Concept**: A filesystem that changes content based on the observer.
- **Mechanism**: Each inode has an `i_extra` field filled with entropy (keystrokes, mouse movement).
- **Mutation**: `read()` operations XOR the file content with this entropy, ensuring that two users reading the same file see different data. `write()` updates the entropy.

### Layer 3: Ego Containers
- **Concept**: Stronger isolation than Linux namespaces, based on "Egos".
- **Mechanism**: An `Ego` is a container with a `BitMap` of visible PIDs.
- **Merge**: `syscall_ego_merge()` allows one ego to absorb the visibility of another, effectively "becoming god" over those processes.

### Layer 4: AbyssHyper (Type-0 Hypervisor)
- **Concept**: The kernel runs *inside* a minimal hypervisor to allow for "Ring -1" operations.
- **Mechanism**: A thin assembly shim (`hyper.S`) installs VMX/SVM/H-extension. `syscall_eject()` drops the kernel out of guest mode into hypervisor mode for live patching.

### Layer 5: Predictive Shell
- **Concept**: A shell that knows what you want to type before you do.
- **Mechanism**: A 16MB Markov Chain table tracks trigram frequencies of all keystrokes.
- **Prediction**: Pressing `Tab` on an empty line predicts the next 5 characters. Deviating from the prediction too often triggers a "Free Will Violation" reboot.

### Layer 6: Quantum-Entangled Files
- **Concept**: Files that are correlated across different machines.
- **Mechanism**: `/dev/quantum0` and `/dev/quantum1` use a "violation mask" (simulated from Bell-test data) to XOR data. Writing to one "entangles" it, and reading from the other reveals the correlated state.

### Layer 7: Self-Modifying Page Tables
- **Concept**: User-space control over kernel memory mapping.
- **Mechanism**: Page tables are mapped to a fixed address (`0xffff888000000000`). `mmap()`ing this allows a process to rewrite its own page tables, enabling tricks like instant Ring 0 jumps (`lain-goto`).

### Layer 8: Ascension Trigger
- **Concept**: An "end game" for the OS.
- **Mechanism**: A global atomic counter at `0x90000000`. When it hits 777, the kernel uploads its entire state to IPFS, overwrites the MBR with a farewell message, and executes a triple fault to reboot.

### Layer 9: Live Decompiler & Hot Patching
- **Concept**: No need to stop the world to fix a bug.
- **Mechanism**: `syscall_decompile` generates C source from running machine code. `syscall_hotpatch` overwrites running code in memory, fixing relocations on the fly.

### Layer 10: Triple-ISA Fat Binary
- **Concept**: One binary, three architectures.
- **Mechanism**: The linker script creates sections for `.text.riscv64`, `.text.x86_64`, and `.text.aarch64`. The bootloader jumps to the correct one, allowing the same file to boot on any supported CPU.
