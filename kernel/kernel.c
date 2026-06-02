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

#define MAIN_MENU_GAME_1 0
#define MAIN_MENU_GAME_2 1
#define MAIN_MENU_EXIT 2

#define GAME_OVER_PLAY_AGAIN 0
#define GAME_OVER_EXIT 1

static void write_text_at(const char* text, int x, int y, unsigned char color) {
    int i;

    i = 0;
    while (text[i] != '\0') {
        vga_put_at(text[i], x + i, y, color);
        i++;
    }
}

static void draw_main_menu(int selected_option) {
    vga_clear();

    write_text_at("MiniGameOS", 32, 5, 0x0F);
    write_text_at("Use W/S to move", 28, 8, 0x07);
    write_text_at("Press Enter or Space", 27, 9, 0x07);

    if (selected_option == MAIN_MENU_GAME_1) {
        write_text_at("> Game 1", 32, 12, 0x0A);
    } else {
        write_text_at("  Game 1", 32, 12, 0x0F);
    }

    if (selected_option == MAIN_MENU_GAME_2) {
        write_text_at("> Game 2", 32, 13, 0x0A);
    } else {
        write_text_at("  Game 2", 32, 13, 0x0F);
    }

    if (selected_option == MAIN_MENU_EXIT) {
        write_text_at("> Exit Game", 32, 14, 0x0A);
    } else {
        write_text_at("  Exit Game", 32, 14, 0x0F);
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

static int get_main_menu_choice(void) {
    int selected_option;
    int prev_w;
    int prev_s;
    int prev_enter;
    int prev_space;

    selected_option = MAIN_MENU_GAME_1;
    keyboard_poll();
    prev_w = keyboard_is_down(KEY_W);
    prev_s = keyboard_is_down(KEY_S);
    prev_enter = keyboard_is_down(KEY_ENTER);
    prev_space = keyboard_is_down(KEY_SPACE);

    draw_main_menu(selected_option);

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

        if (w && !prev_w && selected_option > MAIN_MENU_GAME_1) {
            selected_option--;
            draw_main_menu(selected_option);
        }

        if (s && !prev_s && selected_option < MAIN_MENU_EXIT) {
            selected_option++;
            draw_main_menu(selected_option);
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
    int menu_choice;

    //idt_init();
    //timer_init(50);

    while (1) {
        menu_choice = get_main_menu_choice();

        if (menu_choice == MAIN_MENU_GAME_1) {
            run_game(1);
        }

        if (menu_choice == MAIN_MENU_GAME_2) {
            run_game(2);
        }

        if (menu_choice == MAIN_MENU_EXIT) {
            show_exit_screen();
            while (1) {
            }
        }
    }
}
