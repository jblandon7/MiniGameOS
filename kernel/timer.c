#include "timer.h"
#include "pic.h"
#include "io.h"

static unsigned int ticks = 0;

void timer_init(unsigned int frequency) {
    unsigned int divisor;
    unsigned char low;
    unsigned char high;

    divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    low = divisor & 0xFF;
    high = (divisor >> 8) & 0xFF;

    outb(0x40, low);
    outb(0x40, high);
}

void timer_handler(void) {
    ticks++;
    pic_send_eoi(0);
}

unsigned int timer_get_ticks(void) {
    return ticks;
}
