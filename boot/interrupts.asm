global idt_load
global irq0_stub
global irq1_stub
global enable_interrupts
global disable_interrupts

extern timer_handler
extern keyboard_handler

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

irq0_stub:
    pusha
    call timer_handler
    popa
    iretd

irq1_stub:
    pusha
    call keyboard_handler
    popa
    iretd
