#include "vga.h"
#include "idt.h"
#include "timer.h"
#include "game.h"
#include "keyboard.h"
extern void enable_interrupts(void);

void kernel_main(void) {
    vga_clear();
    vga_print("Booting MiniGameOS...\n");

    //idt_init();
    //timer_init(50);

    game_init();
    //game_draw();

    //enable_interrupts();

    //game_run_demo();

    while (1) {
    	keyboard_poll();
	game_update();
	game_draw();
	}
}
