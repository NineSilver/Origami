ENTRY(kmain)
SECTIONS
{
    . = 0xffffffff80200000;
    .origamihdr : ALIGN(4K) {
        KEEP(*(.origamihdr))
    }

    .text : ALIGN(4K) {
        *(.text*)
    }

    .data : ALIGN(4K) {
        *(.data*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata*)
    }

    .bss : ALIGN(4K) {
        *(.bss*)
        *(COMMON)
    }
}