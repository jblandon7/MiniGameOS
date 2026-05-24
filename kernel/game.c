#include "game.h"
#include "vga.h"
#include "keyboard.h"
#include "timer.h"

#define GAME_LEFT 2
#define GAME_TOP 4
#define GAME_WIDTH 50
#define GAME_HEIGHT 18

#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20

static int old_player_x;
static int old_player_y;
static int old_enemy_x;
static int old_enemy_y;
static int player_x;
static int player_y;
static int enemy_x;
static int enemy_y;
static int score;
static int game_over;
static void draw_border(void);
static void clear_game_area(void);
static int prev_w;
static int prev_a;
static int prev_s;
static int prev_d;

void game_init(void) {
    
    prev_w = 0;
    prev_a = 0;
    prev_s = 0;
    prev_d = 0;

    player_x = 5;
    player_y = 9;

    enemy_x = GAME_WIDTH - 3;
    enemy_y = 9;

    old_player_x = player_x;
    old_player_y = player_y;
    old_enemy_x = enemy_x;
    old_enemy_y = enemy_y;
    
    score = 0;
    game_over = 0;
    vga_clear();
    vga_print("MiniGameOS - Polling 2D Game\n");
    vga_print("WASD to move. Avoid X.\n");

    draw_border();

}

static void draw_border(void) {
    int x;
    int y;

    for (x = 0; x < GAME_WIDTH; x++) {
        vga_put_at('#', GAME_LEFT + x, GAME_TOP, 0x0F);
        vga_put_at('#', GAME_LEFT + x, GAME_TOP + GAME_HEIGHT - 1, 0x0F);
    }

    for (y = 0; y < GAME_HEIGHT; y++) {
        vga_put_at('#', GAME_LEFT, GAME_TOP + y, 0x0F);
        vga_put_at('#', GAME_LEFT + GAME_WIDTH - 1, GAME_TOP + y, 0x0F);
    }
}

static void clear_game_area(void) {
    int x;
    int y;

    for (y = 1; y < GAME_HEIGHT - 1; y++) {
        for (x = 1; x < GAME_WIDTH - 1; x++) {
            vga_put_at(' ', GAME_LEFT + x, GAME_TOP + y, 0x00);
        }
    }
}

void game_update(void) {
    keyboard_poll();

    old_player_x = player_x;
    old_player_y = player_y;
    old_enemy_x = enemy_x;
    old_enemy_y = enemy_y;

    if (game_over) {
        return;
    }


    int w;
    int a;
    int s;
    int d;

    w = keyboard_is_down(KEY_W);
    a = keyboard_is_down(KEY_A);
    s = keyboard_is_down(KEY_S);
    d = keyboard_is_down(KEY_D);
	    if (w && !prev_w && player_y > 1) {
	    player_y--;
	}

	if (s && !prev_s && player_y < GAME_HEIGHT - 2) {
	    player_y++;
	}

	if (a && !prev_a && player_x > 1) {
	    player_x--;
	}

	if (d && !prev_d && player_x < GAME_WIDTH - 2) {
    	player_x++;
	}

	prev_w = w;
	prev_a = a;
	prev_s = s;
	prev_d = d;

/*
    if (keyboard_is_down(KEY_W) && player_y > 1) {
        player_y--;
    }

    if (keyboard_is_down(KEY_S) && player_y < GAME_HEIGHT - 2) {
        player_y++;
    }

    if (keyboard_is_down(KEY_A) && player_x > 1) {
        player_x--;
    }

    if (keyboard_is_down(KEY_D) && player_x < GAME_WIDTH - 2) {
        player_x++;
    }

    enemy_x--;

    if (enemy_x <= 1) {
        enemy_x = GAME_WIDTH - 2;
        enemy_y++;

        if (enemy_y >= GAME_HEIGHT - 1) {
            enemy_y = 1;
        }
    }
	*/

    if (player_x == enemy_x && player_y == enemy_y) {
        game_over = 1;
    }
	
    // score++;
}
void game_draw(void) {
    //vga_clear();

    //vga_print("MiniGameOS - Interrupt Driven 2D Game\n");
    //vga_print("WASD to move. Avoid X. Score: ");
    //vga_print_int(score);

    //draw_border();
    //clear_game_area();
    //vga_put_at(' ', GAME_LEFT + old_player_x, GAME_TOP + old_player_y, 0x00);
    //vga_put_at(' ', GAME_LEFT + old_enemy_x, GAME_TOP + old_enemy_y, 0x00);

    if (old_player_x != player_x || old_player_y != player_y) {
        vga_put_at(' ', GAME_LEFT + old_player_x, GAME_TOP + old_player_y, 0x00);
    }

    if (old_enemy_x != enemy_x || old_enemy_y != enemy_y) {
        vga_put_at(' ', GAME_LEFT + old_enemy_x, GAME_TOP + old_enemy_y, 0x00);
    }

    if (game_over) {
        vga_put_at('G', GAME_LEFT + 18, GAME_TOP + 8, 0x0C);
        vga_put_at('A', GAME_LEFT + 19, GAME_TOP + 8, 0x0C);
        vga_put_at('M', GAME_LEFT + 20, GAME_TOP + 8, 0x0C);
        vga_put_at('E', GAME_LEFT + 21, GAME_TOP + 8, 0x0C);

        vga_put_at('O', GAME_LEFT + 23, GAME_TOP + 8, 0x0C);
        vga_put_at('V', GAME_LEFT + 24, GAME_TOP + 8, 0x0C);
        vga_put_at('E', GAME_LEFT + 25, GAME_TOP + 8, 0x0C);
        vga_put_at('R', GAME_LEFT + 26, GAME_TOP + 8, 0x0C);
        return;
    }

    vga_put_at('@', GAME_LEFT + player_x, GAME_TOP + player_y, 0x0A);
    vga_put_at('X', GAME_LEFT + enemy_x, GAME_TOP + enemy_y, 0x0C);
}

void game_run_demo(void) {
    unsigned int last_tick;

    last_tick = timer_get_ticks();

    while (1) {
        if (timer_get_ticks() != last_tick) {
            last_tick = timer_get_ticks();

                game_update();
                game_draw();
            
        }
    }
}


