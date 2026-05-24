#include "keyboard.h"
#include "pic.h"
#include "io.h"
#include "vga.h"

static int key_down[128];

static void keyboard_process_scancode(unsigned char scancode) {
    unsigned char key;

    //vga_put_at('K', 70, 0, 0x0E);

    if (scancode < 128) {
        key_down[scancode] = 1;
    } else {
        key = scancode - 128;

        if (key < 128) {
            key_down[key] = 0;
        }
    }
}

void keyboard_handler(void) {
    unsigned char scancode;

    scancode = inb(0x60);
    keyboard_process_scancode(scancode);

    pic_send_eoi(1);
}

void keyboard_poll(void) {
    unsigned char status;
    unsigned char scancode;

    status = inb(0x64);

    if (status & 1) {
        scancode = inb(0x60);
        keyboard_process_scancode(scancode);
    }
}

int keyboard_is_down(unsigned char scancode) {
    if (scancode >= 128) {
        return 0;
    }

    return key_down[scancode];
}
