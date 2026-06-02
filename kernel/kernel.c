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

static void write_text_at(const char* text, int x, int y, unsigned char color) {
    int i;

    i = 0;
    while (text[i] != '\0') {
        vga_put_at(text[i], x + i, y, color);
        i++;
    }
}

static void draw_start_menu(int selected_option) {
    vga_clear();

    write_text_at("MiniGameOS", 32, 5, 0x0F);
    write_text_at("Use W/S to move", 28, 8, 0x07);
    write_text_at("Press Enter or Space", 27, 9, 0x07);

    if (selected_option == 0) {
        write_text_at("> Start Game", 30, 12, 0x0A);
        write_text_at("  Exit Game", 30, 13, 0x0F);
    } else {
        write_text_at("  Start Game", 30, 12, 0x0F);
        write_text_at("> Exit Game", 30, 13, 0x0A);
    }
}

static int menu_get_choice(void) {
    int selected_option;
    int prev_w;
    int prev_s;
    int prev_enter;
    int prev_space;

    selected_option = 0;
    prev_w = 0;
    prev_s = 0;
    prev_enter = 0;
    prev_space = 0;

    draw_start_menu(selected_option);

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
            selected_option = 0;
            draw_start_menu(selected_option);
        }

        if (s && !prev_s) {
            selected_option = 1;
            draw_start_menu(selected_option);
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

static void show_exit_screen(void) {
    vga_clear();
    write_text_at("Thanks for playing.", 30, 10, 0x0F);
    write_text_at("System halted.", 32, 12, 0x07);
}

void kernel_main(void) {
    int menu_choice;

    //idt_init();
    //timer_init(50);

    menu_choice = menu_get_choice();

    if (menu_choice == 1) {
        show_exit_screen();
        while (1) {
        }
    }

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
