#include "keyboard.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

int keyboard_is_key_pressed(void) {
    unsigned char status = inb(0x64);
    return status & 1;
}

unsigned char keyboard_read_scancode(void) {
    return inb(0x60);
}
