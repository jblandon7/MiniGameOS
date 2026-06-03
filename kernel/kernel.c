#include "vga.h"
#include "graphics.h"
#include "idt.h"
#include "timer.h"
#include "game.h"
#include "keyboard.h"
extern void enable_interrupts(void);

#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_ENTER 0x1C
#define KEY_SPACE 0x39
#define KEY_1 0x02
#define KEY_2 0x03

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

static void draw_text_launcher_card(int x, int y, char icon, const char* title, int selected) {
    unsigned char border_color;

    if (selected) {
        border_color = 0x0C;
    } else {
        border_color = 0x08;
    }

    vga_put_at('+', x, y, border_color);
    vga_put_at('+', x + 12, y, border_color);
    vga_put_at('+', x, y + 6, border_color);
    vga_put_at('+', x + 12, y + 6, border_color);

    vga_put_at(icon, x + 6, y + 3, 0x0C);
    write_text_at(title, x + 3, y + 8, 0x0F);
}

static void draw_graphics_card(int x, int y, char icon, const char* title, int selected) {
    unsigned int border_color;
    unsigned int fill_color;

    fill_color = GFX_ARMOR_GRAY;

    if (selected) {
        border_color = GFX_DEEP_RED;
    } else {
        border_color = GFX_SMOKY_GRAY;
    }

    draw_rect(x + 8, y + 10, 190, 190, GFX_CHARCOAL);
    draw_rect(x, y, 190, 190, fill_color);
    draw_border_rect(x, y, 190, 190, 5, border_color);
    draw_rect(x + 18, y + 18, 154, 112, GFX_BURGUNDY);
    draw_border_rect(x + 18, y + 18, 154, 112, 3, GFX_BLOOD_RED);

    graphics_draw_char(x + 72, y + 44, icon, GFX_LIGHT_TEXT, 12);
    graphics_draw_text(x + 48, y + 146, title, GFX_LIGHT_TEXT, 3);
}

static void draw_graphics_exit_button(int selected) {
    unsigned int border_color;

    if (selected) {
        border_color = GFX_DEEP_RED;
    } else {
        border_color = GFX_SMOKY_GRAY;
    }

    draw_rect(230, 355, 180, 56, GFX_ARMOR_GRAY);
    draw_border_rect(230, 355, 180, 56, 4, border_color);
    graphics_draw_text(266, 373, "Exit Game", GFX_LIGHT_TEXT, 3);
}

static void draw_main_menu(int selected_option) {
    if (!graphics_is_available()) {
        vga_clear();
        write_text_at("MINIgame OS", 34, 3, 0x0C);
        draw_text_launcher_card(20, 8, '@', "Game 1", selected_option == MAIN_MENU_GAME_1);
        draw_text_launcher_card(47, 8, 'X', "Game 2", selected_option == MAIN_MENU_GAME_2);

        if (selected_option == MAIN_MENU_EXIT) {
            write_text_at("> Exit Game", 34, 20, 0x0C);
        } else {
            write_text_at("  Exit Game", 34, 20, 0x0F);
        }

        write_text_at("A/D or W/S to move. Enter or Space to select.", 15, 23, 0x08);
        return;
    }

    clear_screen(GFX_BLACK);
    draw_rect(0, 0, graphics_width(), 60, GFX_CHARCOAL);
    draw_rect(0, 60, graphics_width(), 4, GFX_BURGUNDY);

    graphics_draw_text(188, 34, "MINIgame OS", GFX_LIGHT_TEXT, 4);
    graphics_draw_text(130, 86, "SELECT A GAME", GFX_SMOKY_GRAY, 2);

    draw_graphics_card(105, 125, '@', "Game 1", selected_option == MAIN_MENU_GAME_1);
    draw_graphics_card(345, 125, 'X', "Game 2", selected_option == MAIN_MENU_GAME_2);
    draw_graphics_exit_button(selected_option == MAIN_MENU_EXIT);

    graphics_draw_text(92, 440, "A/D OR W/S MOVE   ENTER/SPACE SELECT   1 GAME 1   2 GAME 2", GFX_SMOKY_GRAY, 2);
}

static int get_main_menu_choice(void) {
    int selected_option;
    int prev_w;
    int prev_a;
    int prev_s;
    int prev_d;
    int prev_enter;
    int prev_space;
    int prev_1;
    int prev_2;

    selected_option = MAIN_MENU_GAME_1;
    keyboard_poll();
    prev_w = keyboard_is_down(KEY_W);
    prev_a = keyboard_is_down(KEY_A);
    prev_s = keyboard_is_down(KEY_S);
    prev_d = keyboard_is_down(KEY_D);
    prev_enter = keyboard_is_down(KEY_ENTER);
    prev_space = keyboard_is_down(KEY_SPACE);
    prev_1 = keyboard_is_down(KEY_1);
    prev_2 = keyboard_is_down(KEY_2);

    draw_main_menu(selected_option);

    while (1) {
        int w;
        int a;
        int s;
        int d;
        int enter;
        int space;
        int key_1;
        int key_2;

        keyboard_poll();

        w = keyboard_is_down(KEY_W);
        a = keyboard_is_down(KEY_A);
        s = keyboard_is_down(KEY_S);
        d = keyboard_is_down(KEY_D);
        enter = keyboard_is_down(KEY_ENTER);
        space = keyboard_is_down(KEY_SPACE);
        key_1 = keyboard_is_down(KEY_1);
        key_2 = keyboard_is_down(KEY_2);

        if (key_1 && !prev_1) {
            return MAIN_MENU_GAME_1;
        }

        if (key_2 && !prev_2) {
            return MAIN_MENU_GAME_2;
        }

        if ((w && !prev_w) || (a && !prev_a)) {
            selected_option--;

            if (selected_option < MAIN_MENU_GAME_1) {
                selected_option = MAIN_MENU_EXIT;
            }

            draw_main_menu(selected_option);
        }

        if ((s && !prev_s) || (d && !prev_d)) {
            selected_option++;

            if (selected_option > MAIN_MENU_EXIT) {
                selected_option = MAIN_MENU_GAME_1;
            }

            draw_main_menu(selected_option);
        }

        if ((enter && !prev_enter) || (space && !prev_space)) {
            return selected_option;
        }

        prev_w = w;
        prev_a = a;
        prev_s = s;
        prev_d = d;
        prev_enter = enter;
        prev_space = space;
        prev_1 = key_1;
        prev_2 = key_2;
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

void kernel_main(unsigned int multiboot_magic, unsigned int multiboot_info_addr) {
    int menu_choice;

    graphics_init(multiboot_magic, multiboot_info_addr);

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
