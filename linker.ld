OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80200000;
    
    .text : {
        *(.text.init)
        *(.text .text.*)
        PROVIDE(etext = .);
    }
    
    .rodata : {
        . = ALIGN(16);
        *(.rodata .rodata.*)
    }
    
    .data : {
        . = ALIGN(16);
        *(.data .data.*)
    }
    
    .bss : {
        . = ALIGN(16);
        PROVIDE(__bss_start = .);
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(__bss_end = .);
    }
    
    PROVIDE(end = .);
    
    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
    }
}
