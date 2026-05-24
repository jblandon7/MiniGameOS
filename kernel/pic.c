#include "pic.h"
#include "io.h"

#define PIC1_COMMAND 0x20                     // Master PIC command port
#define PIC1_DATA    0x21                     // Master PIC data port
#define PIC2_COMMAND 0xA0                     // Slave PIC command port
#define PIC2_DATA    0xA1                     // Slave PIC data port

#define PIC_EOI      0x20                     // End Of Interrupt signal

void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) {                           // IRQ8-IRQ15 come from the slave PIC
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);              // Always notify the master PIC
}

void pic_remap(void) {
    unsigned char a1;
    unsigned char a2;

    a1 = inb(PIC1_DATA);                      // Save current PIC masks
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);                 // Begin PIC initialization
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    outb(PIC1_DATA, 0x20);                    // Master IRQs become interrupts 0x20-0x27
    io_wait();
    outb(PIC2_DATA, 0x28);                    // Slave IRQs become interrupts 0x28-0x2F
    io_wait();

    outb(PIC1_DATA, 0x04);                    // Tell master that slave is on IRQ2
    io_wait();
    outb(PIC2_DATA, 0x02);                    // Tell slave its cascade identity
    io_wait();

    outb(PIC1_DATA, 0x01);                    // Set both PICs to 8086 mode
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, a1);                      // Restore saved masks
    outb(PIC2_DATA, a2);

    outb(PIC1_DATA, 0xFC);
    outb(PIC2_DATA, 0xFF);
}
