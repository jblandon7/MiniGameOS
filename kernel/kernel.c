#include "vga.h"
#include "game.h"

void kernel_main(void) {
    	vga_clear();
	vga_print("Welcome to MiniGameOS\n");
	vga_print("Kernel loaded successfullly.\n");

	game_run_demo();

	while(1){
	}
}
