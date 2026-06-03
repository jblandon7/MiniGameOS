section .multiboot
    ; GRUB checks this section to see if this file is a bootable kernel

    dd 0x1BADB002                              ; Password telling GRUB this is a bootable kernel
    dd 0x07                                    ; Align modules, request memory info, and ask for graphics mode
    dd -(0x1BADB002 + 0x07)                    ; magic + flags + checksum must equal 0
    dd 0                                       ; 0 means linear framebuffer graphics mode
    dd 640                                     ; Preferred framebuffer width
    dd 480                                     ; Preferred framebuffer height
    dd 32                                      ; Preferred framebuffer depth

section .bss                                  ; Uninitialized memory/storage
align 16
stack_bottom:
    resb 16384
stack_top:

section .text                                 ; where CPU instructions go
global start                                  ; Mark of the kernel entry point visible to linker
extern kernel_main                            ; kernel_main is defined in kernel.c

; GRUB puts a magic number in eax and the Multiboot info address in ebx.
start:
    mov esp, stack_top
    push ebx                                  ; Second C argument: multiboot info address
    push eax                                  ; First C argument: multiboot magic number
    call kernel_main                          ; Assembly hands control to kernel_main()

; Stop for when kernel_main finishes
hang:
    cli
    hlt                                       ; Tells the CPU to sleep until an interrupt wakes it up
    jmp hang                                  ; Loop back to sleep again
