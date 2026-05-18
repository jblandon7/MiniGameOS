#include "game.h"
#include "vga.h"

#define GAME_WIDTH 20
#define GAME_HEIGHT 10

static int player_x = 5;
static int player_y = 5;
static int enemy_x = 15;
static int enemy_y = 5;
static int score = 0;
static int game_over = 0;

void game_init(void) {
    player_x = 10;
    player_y = 10;

    enemy_x = 20;
    enemy_y = 10;

    score = 0;
    game_over = 0;
}

void game_update(void) {
	if(game_over){
		return;
	}

    if (keyboard_is_key_pressed()) {
        unsigned char key = keyboard_read_scancode();

        if (key == 0x11 && player_y > 0) {
            player_y--;
        } else if (key == 0x1F && player_y < GAME_HEIGHT - 1) {
            player_y++;
        } else if (key == 0x1E && player_x > 0) {
            player_x--;
        } else if (key == 0x20 && player_x < GAME_WIDTH - 1) {
            player_x++;
        }
    }

    enemy_x--;

    if (enemy_x < 0) {
        enemy_x = GAME_WIDTH - 1;
    }

    score++;

	if (player_x == enemy_x && player_y == enemy_y) {
        	game_over = 1;
    	}
}

void game_draw(void) {
    if (game_over) {
    	vga_clear();
    	vga_print("GAME OVER\n");
    	vga_print("Final Score: ");
    	vga_print_int(score);
    	vga_print("\n");
    	return;
	}

    int y;
    int x;
    vga_clear();

    vga_print("MiniGameOS 2D Text Game\n");
    vga_print("Score: ");
    vga_print_int(score);
    vga_print("\n\n");

    for (y = 0; y < GAME_HEIGHT; y++) {
        for (x = 0; x < GAME_WIDTH; x++) {
            if (x == player_x && y == player_y) {
                vga_put_char('P');
            } else if (x == enemy_x && y == enemy_y) {
                vga_put_char('E');
            } else {
                vga_put_char('.');
            }
        }
        vga_put_char('\n');
    }
}

void game_run_demo(void) {
    int i;

    game_init();

    for (i = 0; i < 100000; i++) {
        if (i % 5000 == 0) {
            game_update();
            game_draw();
        }
    }
}

