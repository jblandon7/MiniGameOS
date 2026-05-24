#include "idt.h"
#include "pic.h"

struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

extern void idt_load(struct idt_ptr* ptr);
extern void irq0_stub(void);
extern void irq1_stub(void);

static struct idt_entry idt[256];
static struct idt_ptr idtp;

static void idt_set_gate(int num, unsigned int base, unsigned short selector, unsigned char flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
    idt[num].base_high = (base >> 16) & 0xFFFF;
}

void idt_init(void) {
    int i;

    for (i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    pic_remap();

    idt_set_gate(32, (unsigned int)irq0_stub, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq1_stub, 0x08, 0x8E);

    idtp.limit = sizeof(idt) - 1;
    idtp.base = (unsigned int)&idt;

    idt_load(&idtp);
}




