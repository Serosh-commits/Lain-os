/*
 * Triple-ISA Fat Binary Linker Script Fragment
 * This would be included in the main linker script to create the sections.
 */

SECTIONS
{
    .text.riscv64 : {
        *(.text.riscv64)
    }

    .text.x86_64 : {
        *(.text.x86_64)
    }

    .text.aarch64 : {
        *(.text.aarch64)
    }
}
