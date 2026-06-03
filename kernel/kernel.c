#include "vga.h"
#include "idt.h"
#include "timer.h"
#include "game.h"
#include "keyboard.h"
extern void enable_interrupts(void);

#define KEY_W 0x11
#define KEY_S 0x1F
#define KEY_ENTER 0x1C
#define KEY_SPACE 0x39
#define KEY_1 0x02
#define KEY_2 0x03
#define KEY_L 0x26
#define KEY_O 0x18

#define START_ACTION_GAME_1 1
#define START_ACTION_GAME_2 2
#define START_ACTION_LOG 3
#define START_ACTION_OPTIMIZER 4

#define GAME_OVER_PLAY_AGAIN 0
#define GAME_OVER_EXIT 1

#define COLOR_BLACK 0x00
#define COLOR_DARK_GRAY 0x08
#define COLOR_GRAY 0x07
#define COLOR_RED 0x04
#define COLOR_LIGHT_RED 0x0C

static void write_text_at(const char* text, int x, int y, unsigned char color) {
    int i;

    i = 0;
    while (text[i] != '\0') {
        vga_put_at(text[i], x + i, y, color);
        i++;
    }
}

static void draw_box(int left, int top, int width, int height, unsigned char color) {
    int x;
    int y;

    for (x = 0; x < width; x++) {
        vga_put_at('#', left + x, top, color);
        vga_put_at('#', left + x, top + height - 1, color);
    }

    for (y = 0; y < height; y++) {
        vga_put_at('#', left, top + y, color);
        vga_put_at('#', left + width - 1, top + y, color);
    }
}

static void fill_box(int left, int top, int width, int height, unsigned char color) {
    int x;
    int y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            vga_put_at(' ', left + x, top + y, color);
        }
    }
}

static void draw_game_card(int left, int top, char icon, const char* title) {
    fill_box(left + 1, top + 1, 13, 7, COLOR_BLACK);
    draw_box(left + 1, top + 1, 13, 7, COLOR_DARK_GRAY);
    draw_box(left, top, 13, 7, COLOR_RED);

    vga_put_at(icon, left + 6, top + 3, COLOR_LIGHT_RED);
    write_text_at(title, left + 3, top + 8, COLOR_GRAY);
}

static void draw_start_screen(void) {
    vga_clear();

    write_text_at("MINI", 34, 3, COLOR_RED);
    write_text_at("game OS", 38, 3, COLOR_GRAY);

    draw_game_card(20, 8, '@', "Game 1");
    draw_game_card(47, 8, 'X', "Game 2");

    write_text_at("1 Start/Resume Game 1", 12, 20, COLOR_GRAY);
    write_text_at("2 Start/Resume Game 2", 43, 20, COLOR_GRAY);
    write_text_at("L AI Decision Log", 12, 22, COLOR_RED);
    write_text_at("O AI Memory Optimizer", 43, 22, COLOR_RED);
}

static void wait_for_menu_return(void) {
    int prev_enter;
    int prev_space;

    keyboard_poll();
    prev_enter = keyboard_is_down(KEY_ENTER);
    prev_space = keyboard_is_down(KEY_SPACE);

    while (1) {
        int enter;
        int space;

        keyboard_poll();

        enter = keyboard_is_down(KEY_ENTER);
        space = keyboard_is_down(KEY_SPACE);

        if ((enter && !prev_enter) || (space && !prev_space)) {
            return;
        }

        prev_enter = enter;
        prev_space = space;
    }
}

static void show_ai_decision_log(void) {
    vga_clear();

    write_text_at("AI Decision Log", 31, 5, COLOR_RED);
    write_text_at("No AI decisions recorded yet.", 24, 10, COLOR_GRAY);
    write_text_at("Press Enter or Space to return.", 22, 15, COLOR_DARK_GRAY);

    wait_for_menu_return();
}

static void show_ai_memory_optimizer(void) {
    vga_clear();

    write_text_at("AI Memory Optimizer", 29, 5, COLOR_RED);
    write_text_at("Memory optimizer ready.", 28, 10, COLOR_GRAY);
    write_text_at("No memory cleanup needed yet.", 25, 12, COLOR_GRAY);
    write_text_at("Press Enter or Space to return.", 22, 15, COLOR_DARK_GRAY);

    wait_for_menu_return();
}

static int get_start_action(void) {
    int prev_1;
    int prev_2;
    int prev_l;
    int prev_o;

    keyboard_poll();
    prev_1 = keyboard_is_down(KEY_1);
    prev_2 = keyboard_is_down(KEY_2);
    prev_l = keyboard_is_down(KEY_L);
    prev_o = keyboard_is_down(KEY_O);

    draw_start_screen();

    while (1) {
        int key_1;
        int key_2;
        int key_l;
        int key_o;

        keyboard_poll();

        key_1 = keyboard_is_down(KEY_1);
        key_2 = keyboard_is_down(KEY_2);
        key_l = keyboard_is_down(KEY_L);
        key_o = keyboard_is_down(KEY_O);

        if (key_1 && !prev_1) {
            return START_ACTION_GAME_1;
        }

        if (key_2 && !prev_2) {
            return START_ACTION_GAME_2;
        }

        if (key_l && !prev_l) {
            return START_ACTION_LOG;
        }

        if (key_o && !prev_o) {
            return START_ACTION_OPTIMIZER;
        }

        prev_1 = key_1;
        prev_2 = key_2;
        prev_l = key_l;
        prev_o = key_o;
    }
}

static void draw_game_over_menu(int selected_option, int game_number) {
    vga_clear();

    if (game_number == 2) {
        write_text_at("Game 2 - GAME OVER", 30, 6, 0x0C);
    } else {
        write_text_at("Game 1 - GAME OVER", 30, 6, 0x0C);
    }

    write_text_at("Use W/S to move", 28, 9, 0x07);
    write_text_at("Press Enter or Space", 27, 10, 0x07);

    if (selected_option == GAME_OVER_PLAY_AGAIN) {
        write_text_at("> Play Again", 31, 13, 0x0A);
        write_text_at("  Exit Game", 31, 14, 0x0F);
    } else {
        write_text_at("  Play Again", 31, 13, 0x0F);
        write_text_at("> Exit Game", 31, 14, 0x0A);
    }
}

static int get_game_over_choice(int game_number) {
    int selected_option;
    int prev_w;
    int prev_s;
    int prev_enter;
    int prev_space;

    selected_option = GAME_OVER_PLAY_AGAIN;
    keyboard_poll();
    prev_w = keyboard_is_down(KEY_W);
    prev_s = keyboard_is_down(KEY_S);
    prev_enter = keyboard_is_down(KEY_ENTER);
    prev_space = keyboard_is_down(KEY_SPACE);

    draw_game_over_menu(selected_option, game_number);

    while (1) {
        int w;
        int s;
        int enter;
        int space;

        keyboard_poll();

        w = keyboard_is_down(KEY_W);
        s = keyboard_is_down(KEY_S);
        enter = keyboard_is_down(KEY_ENTER);
        space = keyboard_is_down(KEY_SPACE);

        if (w && !prev_w) {
            selected_option = GAME_OVER_PLAY_AGAIN;
            draw_game_over_menu(selected_option, game_number);
        }

        if (s && !prev_s) {
            selected_option = GAME_OVER_EXIT;
            draw_game_over_menu(selected_option, game_number);
        }

        if ((enter && !prev_enter) || (space && !prev_space)) {
            return selected_option;
        }

        prev_w = w;
        prev_s = s;
        prev_enter = enter;
        prev_space = space;
    }
}

static void run_game(int game_number) {
    int game_over_choice;

    while (1) {
        game_init(game_number);

        while (1) {
            keyboard_poll();
            game_update();
            game_draw();

            if (game_is_over()) {
                break;
            }
        }

        game_over_choice = get_game_over_choice(game_number);

        if (game_over_choice == GAME_OVER_EXIT) {
            return;
        }
    }
}

static void show_exit_screen(void) {
    vga_clear();
    write_text_at("Thanks for playing.", 30, 10, 0x0F);
    write_text_at("System halted.", 32, 12, 0x07);
}

void kernel_main(void) {
    int start_action;

    //idt_init();
    //timer_init(50);

    while (1) {
        start_action = get_start_action();

        if (start_action == START_ACTION_GAME_1) {
            run_game(1);
        }

        if (start_action == START_ACTION_GAME_2) {
            run_game(2);
        }

        if (start_action == START_ACTION_LOG) {
            show_ai_decision_log();
        }

        if (start_action == START_ACTION_OPTIMIZER) {
            show_ai_memory_optimizer();
        }
    }
}
