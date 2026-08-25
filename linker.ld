SECTIONS
{
    /* Physical entry point for RPi ARM64 bootloader */
    . = 0x80000;

    /* Code section */
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.*)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata .rodata.*)
    }

    /* Read-write data */
    .data : {
        *(.data .data.*)
    }

    /* Uninitialized data */
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* Double-word count calculation for boot.S clearing loop */
    __bss_size = (__bss_end - __bss_start) >> 3;

    /* Discard metadata */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame)
    }
}