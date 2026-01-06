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
