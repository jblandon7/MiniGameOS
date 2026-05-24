#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_handler(void);
void keyboard_poll(void);
int keyboard_is_down(unsigned char scancode);

#endif
