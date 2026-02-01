# LAIN-OS
> "No matter where you are, everyone is always connected."

![lain](https://media1.tenor.com/m/6R6W1n-LmNMAAAAd/lain-lain-iwakura.gif)

LainOS (Abyss) is my attempt to build a kernel that doesn't just manage hardware, but reflects the chaos of the Wired. It’s a hobbyist, research-grade RISC-V kernel written in C++20. I got tired of how bloated modern operating systems are—they hide the machine from you. This project is about bringing the raw power of the silicon back to the user.

Everything here i tried to built it from scratch. No standard library, no external headers—just me, a linker script, and the RISC-V ISA.

---

###  The Soul of the Machine
I didn't want a "normal" OS. I wanted something that feels alive.

*   **Temporal Fracture Scheduler**: Gone are the days of boring round-robin. Abyss uses a "Timeline" system. Every process exists in its own fracture of time. When you `spawn`, you aren't just forking a PID; you're branching a new reality.
*   **Predictive Shell**: It’s creepy, I know. It uses a 16MB Markov Chain to track your habits. It knows what you’re going to type before you do. Press `Tab` and let the machine speak for you.
*   **NeuroVFS**: A recursive, hierarchical filesystem that supports Unix-style paths. It’s got a built-in "yay" simulated package manager because I wanted that Arch feel even in a custom kernel.

### Tools
I built these because I hate debugging with GDB. Why use an external tool when the kernel can debug itself?

*   **Live Disassembler**: Run `decompile <addr>` in the shell. It will literally pull the machine code from RAM and show you the RISC-V instructions.
*   **Hot Patcher**: Found a bug at runtime? Use `patch <addr> <val>` to overwrite the running kernel memory. No reboots required. 
*   **Ascension Trigger**: The ultimate exit. When you're ready, run `ascend`. The kernel will dump its state and reboot the reality.

---

###  Getting it Running
If you have a RISC-V toolchain, it's literally one command.

1.  **Dependencies**
    *   `riscv64-elf-gcc` / `riscv64-elf-g++`
    *   `qemu-system-riscv64`

2.  **Build & Boot**
    ```bash
    make clean && make run
    ```
    *I've tuned the Makefile to give QEMU 512MB of RAM because the predictive shell's Markov tables are hungry for memory.*

---

### ⌨Commands to Try
Once the prompt `root@abyss#` hits your terminal, try these:

*   `neofetch` - See the ASCII art and system stats.
*   `yay -S htop` - Watch the simulated package manager compile htop into your VFS.
*   `mkdir project && cd project` - Navigate the hierarchical filesystem.
*   `decompile 0x80200000` - Peek at the kernel entry point.
*   `ps` & `kill <pid>` - Manage the temporal fractures.

###  Philosophy
This isn't meant to be a replacement for Linux. It's meant to be a playground. It's for the people who want to jump into the middle of a triple-fault and find out *why* it happened. It's for the people who believe the computer is an extension of the self.

**Present Day. Present Time.**
**HA-HA-HA-HA-HA.**
