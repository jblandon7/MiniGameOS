section .multiboot
    ; GRUB checks this section to see if this file is a bootable kernel

    dd 0x1BADB002                              ; Password telling grub that this is a bootable kernel
    dd 0x00                                    ; Special Instructions for GRUB, flag
    dd -(0x1BADB002 + 0x00)                    ; security checksum, pswd + flag + checksum must equal 0 or else it wont run


section .text					; Where CPU instructions go
global start					; Mark of the kernel entry point visible to linker
extern kernel_main				; kernel_main is defined in another file, kernel.c

; Where the kernel starts running
start:
    call kernel_main				; Assembly hands control of the C function kernel_main()


; Stop for when kernel main finishes
hang:
    hlt						; Tells the CPU to go to sleep until interrupt wakes it up
    jmp hang					; Loop back to tell the CPU to go back to sleep
