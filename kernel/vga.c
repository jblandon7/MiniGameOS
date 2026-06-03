#include "vga.h"
#include "graphics.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile unsigned short*)0xB8000)

#define CELL_WIDTH 8
#define CELL_HEIGHT 16

static int row = 0;
static int col = 0;
static unsigned char color = 0x0F;

static unsigned short make_vga_entry(char c, unsigned char color) {
    return (unsigned short)c | ((unsigned short)color << 8);
}

static unsigned int vga_color_to_rgb(unsigned char vga_color) {
    switch (vga_color & 0x0F) {
        case 0x00: return 0x050506;
        case 0x01: return 0x0000AA;
        case 0x02: return 0x00AA00;
        case 0x03: return 0x00AAAA;
        case 0x04: return 0x702029;
        case 0x05: return 0xAA00AA;
        case 0x06: return 0xAA5500;
        case 0x07: return 0xD8D8D8;
        case 0x08: return 0x1B1F21;
        case 0x09: return 0x5555FF;
        case 0x0A: return 0x55FF55;
        case 0x0B: return 0x55FFFF;
        case 0x0C: return 0xB8323A;
        case 0x0D: return 0xFF55FF;
        case 0x0E: return 0xFFFF55;
        default: return 0xFFFFFF;
    }
}

static void draw_framebuffer_cell(char c, int x, int y, unsigned char color) {
    unsigned int foreground;
    unsigned int background;
    int px;
    int py;

    px = x * CELL_WIDTH;
    py = y * CELL_HEIGHT;
    foreground = vga_color_to_rgb(color & 0x0F);
    background = vga_color_to_rgb((color >> 4) & 0x0F);

    draw_rect(px, py, CELL_WIDTH, CELL_HEIGHT, background);

    if (c != ' ') {
        graphics_draw_char(px + 1, py + 4, c, foreground, 1);
    }
}

void vga_clear(void) {
    int r;
    int c;

    if (graphics_is_available()) {
        clear_screen(vga_color_to_rgb((color >> 4) & 0x0F));
    } else {
        for (r = 0; r < VGA_HEIGHT; r++) {
            for (c = 0; c < VGA_WIDTH; c++) {
                VGA_MEMORY[r * VGA_WIDTH + c] = make_vga_entry(' ', color);
            }
        }
    }

    row = 0;
    col = 0;
}

void vga_put_at(char c, int x, int y, unsigned char color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return;
    }

    if (graphics_is_available()) {
        draw_framebuffer_cell(c, x, y, color);
    } else {
        VGA_MEMORY[y * VGA_WIDTH + x] = make_vga_entry(c, color);
    }
}

void vga_put_char(char c) {
    if (c == '\n') {
        row++;
        col = 0;
        return;
    }

    vga_put_at(c, col, row, color);
    col++;

    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
    }

    if (row >= VGA_HEIGHT) {
        row = 0;
    }
}

void vga_print(const char* str) {
    int i;

    i = 0;
    while (str[i] != '\0') {
        vga_put_char(str[i]);
        i++;
    }
}

void vga_print_int(int num) {
    char buffer[12];
    int i;
    int j;

    i = 0;

    if (num == 0) {
        vga_put_char('0');
        return;
    }

    if (num < 0) {
        vga_put_char('-');
        num = -num;
    }

    while (num > 0) {
        buffer[i] = '0' + (num % 10);
        num = num / 10;
        i++;
    }

    for (j = i - 1; j >= 0; j--) {
        vga_put_char(buffer[j]);
    }
}
