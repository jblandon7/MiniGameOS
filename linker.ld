/* Tell the linker that the kernel starts at the symbol named start */
ENTRY(start)

/* How the kernel sections should be arranged in memory */
SECTIONS
{
    . = 1M;			/* Start at 1MB ( 0x100000) */ 

    .text :			/* code/instructions that the CPU runs */
    {
        *(.multiboot)		/* Put all the .multiboot section in here (ID) */
        *(.text)		/* .text section ( Where all the executable code go)*/
    }

    .rodata :			/* Read-only section(constant and string literal go)*/
    {
        *(.rodata)		/* Put all read only constants and string here */
    }

    .data :			/* Data section */
    {
        *(.data)		/* Put global/static variables here with value */
    }

    .bss :			/* All unitialized global/static variables here */
    {
        *(.bss)		
    }
}



