#include "graphics.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_INFO_FRAMEBUFFER 0x00001000

struct multiboot_info {
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    unsigned int cmdline;
    unsigned int mods_count;
    unsigned int mods_addr;
    unsigned int syms[4];
    unsigned int mmap_length;
    unsigned int mmap_addr;
    unsigned int drives_length;
    unsigned int drives_addr;
    unsigned int config_table;
    unsigned int boot_loader_name;
    unsigned int apm_table;
    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;
    unsigned int framebuffer_addr_low;
    unsigned int framebuffer_addr_high;
    unsigned int framebuffer_pitch;
    unsigned int framebuffer_width;
    unsigned int framebuffer_height;
    unsigned char framebuffer_bpp;
    unsigned char framebuffer_type;
    unsigned short color_info;
} __attribute__((packed));

static unsigned char* framebuffer;
static unsigned int fb_width;
static unsigned int fb_height;
static unsigned int fb_pitch;
static unsigned char fb_bpp;
static int fb_ready;

static unsigned char glyph_rows(char c, int row) {
    unsigned char rows[7];
    int i;

    for (i = 0; i < 7; i++) {
        rows[i] = 0;
    }

    if (c >= 'a' && c <= 'z') {
        c = c - 32;
    }

    switch (c) {
        case 'A': rows[0]=14; rows[1]=17; rows[2]=17; rows[3]=31; rows[4]=17; rows[5]=17; rows[6]=17; break;
        case 'B': rows[0]=30; rows[1]=17; rows[2]=17; rows[3]=30; rows[4]=17; rows[5]=17; rows[6]=30; break;
        case 'C': rows[0]=14; rows[1]=17; rows[2]=16; rows[3]=16; rows[4]=16; rows[5]=17; rows[6]=14; break;
        case 'D': rows[0]=30; rows[1]=17; rows[2]=17; rows[3]=17; rows[4]=17; rows[5]=17; rows[6]=30; break;
        case 'E': rows[0]=31; rows[1]=16; rows[2]=16; rows[3]=30; rows[4]=16; rows[5]=16; rows[6]=31; break;
        case 'F': rows[0]=31; rows[1]=16; rows[2]=16; rows[3]=30; rows[4]=16; rows[5]=16; rows[6]=16; break;
        case 'G': rows[0]=14; rows[1]=17; rows[2]=16; rows[3]=23; rows[4]=17; rows[5]=17; rows[6]=14; break;
        case 'H': rows[0]=17; rows[1]=17; rows[2]=17; rows[3]=31; rows[4]=17; rows[5]=17; rows[6]=17; break;
        case 'I': rows[0]=14; rows[1]=4; rows[2]=4; rows[3]=4; rows[4]=4; rows[5]=4; rows[6]=14; break;
        case 'J': rows[0]=7; rows[1]=2; rows[2]=2; rows[3]=2; rows[4]=18; rows[5]=18; rows[6]=12; break;
        case 'K': rows[0]=17; rows[1]=18; rows[2]=20; rows[3]=24; rows[4]=20; rows[5]=18; rows[6]=17; break;
        case 'L': rows[0]=16; rows[1]=16; rows[2]=16; rows[3]=16; rows[4]=16; rows[5]=16; rows[6]=31; break;
        case 'M': rows[0]=17; rows[1]=27; rows[2]=21; rows[3]=21; rows[4]=17; rows[5]=17; rows[6]=17; break;
        case 'N': rows[0]=17; rows[1]=25; rows[2]=21; rows[3]=19; rows[4]=17; rows[5]=17; rows[6]=17; break;
        case 'O': rows[0]=14; rows[1]=17; rows[2]=17; rows[3]=17; rows[4]=17; rows[5]=17; rows[6]=14; break;
        case 'P': rows[0]=30; rows[1]=17; rows[2]=17; rows[3]=30; rows[4]=16; rows[5]=16; rows[6]=16; break;
        case 'Q': rows[0]=14; rows[1]=17; rows[2]=17; rows[3]=17; rows[4]=21; rows[5]=18; rows[6]=13; break;
        case 'R': rows[0]=30; rows[1]=17; rows[2]=17; rows[3]=30; rows[4]=20; rows[5]=18; rows[6]=17; break;
        case 'S': rows[0]=15; rows[1]=16; rows[2]=16; rows[3]=14; rows[4]=1; rows[5]=1; rows[6]=30; break;
        case 'T': rows[0]=31; rows[1]=4; rows[2]=4; rows[3]=4; rows[4]=4; rows[5]=4; rows[6]=4; break;
        case 'U': rows[0]=17; rows[1]=17; rows[2]=17; rows[3]=17; rows[4]=17; rows[5]=17; rows[6]=14; break;
        case 'V': rows[0]=17; rows[1]=17; rows[2]=17; rows[3]=17; rows[4]=17; rows[5]=10; rows[6]=4; break;
        case 'W': rows[0]=17; rows[1]=17; rows[2]=17; rows[3]=21; rows[4]=21; rows[5]=21; rows[6]=10; break;
        case 'X': rows[0]=17; rows[1]=17; rows[2]=10; rows[3]=4; rows[4]=10; rows[5]=17; rows[6]=17; break;
        case 'Y': rows[0]=17; rows[1]=17; rows[2]=10; rows[3]=4; rows[4]=4; rows[5]=4; rows[6]=4; break;
        case 'Z': rows[0]=31; rows[1]=1; rows[2]=2; rows[3]=4; rows[4]=8; rows[5]=16; rows[6]=31; break;
        case '0': rows[0]=14; rows[1]=17; rows[2]=19; rows[3]=21; rows[4]=25; rows[5]=17; rows[6]=14; break;
        case '1': rows[0]=4; rows[1]=12; rows[2]=4; rows[3]=4; rows[4]=4; rows[5]=4; rows[6]=14; break;
        case '2': rows[0]=14; rows[1]=17; rows[2]=1; rows[3]=2; rows[4]=4; rows[5]=8; rows[6]=31; break;
        case '3': rows[0]=30; rows[1]=1; rows[2]=1; rows[3]=14; rows[4]=1; rows[5]=1; rows[6]=30; break;
        case '4': rows[0]=2; rows[1]=6; rows[2]=10; rows[3]=18; rows[4]=31; rows[5]=2; rows[6]=2; break;
        case '5': rows[0]=31; rows[1]=16; rows[2]=16; rows[3]=30; rows[4]=1; rows[5]=1; rows[6]=30; break;
        case '6': rows[0]=14; rows[1]=16; rows[2]=16; rows[3]=30; rows[4]=17; rows[5]=17; rows[6]=14; break;
        case '7': rows[0]=31; rows[1]=1; rows[2]=2; rows[3]=4; rows[4]=8; rows[5]=8; rows[6]=8; break;
        case '8': rows[0]=14; rows[1]=17; rows[2]=17; rows[3]=14; rows[4]=17; rows[5]=17; rows[6]=14; break;
        case '9': rows[0]=14; rows[1]=17; rows[2]=17; rows[3]=15; rows[4]=1; rows[5]=1; rows[6]=14; break;
        case '@': rows[0]=14; rows[1]=17; rows[2]=23; rows[3]=21; rows[4]=23; rows[5]=16; rows[6]=14; break;
        case '>': rows[0]=16; rows[1]=8; rows[2]=4; rows[3]=2; rows[4]=4; rows[5]=8; rows[6]=16; break;
        case '.': rows[0]=0; rows[1]=0; rows[2]=0; rows[3]=0; rows[4]=0; rows[5]=12; rows[6]=12; break;
        case '-': rows[0]=0; rows[1]=0; rows[2]=0; rows[3]=31; rows[4]=0; rows[5]=0; rows[6]=0; break;
        case '/': rows[0]=1; rows[1]=1; rows[2]=2; rows[3]=4; rows[4]=8; rows[5]=16; rows[6]=16; break;
        default: break;
    }

    return rows[row];
}

int graphics_init(unsigned int multiboot_magic, unsigned int multiboot_info_addr) {
    struct multiboot_info* info;

    fb_ready = 0;

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return 0;
    }

    info = (struct multiboot_info*)multiboot_info_addr;

    if ((info->flags & MULTIBOOT_INFO_FRAMEBUFFER) == 0) {
        return 0;
    }

    if (info->framebuffer_bpp != 32 || info->framebuffer_type != 1) {
        return 0;
    }

    framebuffer = (unsigned char*)info->framebuffer_addr_low;
    fb_width = info->framebuffer_width;
    fb_height = info->framebuffer_height;
    fb_pitch = info->framebuffer_pitch;
    fb_bpp = info->framebuffer_bpp;
    fb_ready = 1;

    return 1;
}

int graphics_is_available(void) {
    return fb_ready;
}

int graphics_width(void) {
    return fb_width;
}

int graphics_height(void) {
    return fb_height;
}

void draw_pixel(int x, int y, unsigned int color) {
    unsigned int* pixel;

    if (!fb_ready) {
        return;
    }

    if (x < 0 || y < 0 || x >= (int)fb_width || y >= (int)fb_height) {
        return;
    }

    pixel = (unsigned int*)(framebuffer + y * fb_pitch + x * (fb_bpp / 8));
    *pixel = color;
}

void draw_rect(int x, int y, int width, int height, unsigned int color) {
    int px;
    int py;

    for (py = 0; py < height; py++) {
        for (px = 0; px < width; px++) {
            draw_pixel(x + px, y + py, color);
        }
    }
}

void draw_border_rect(int x, int y, int width, int height, int thickness, unsigned int color) {
    draw_rect(x, y, width, thickness, color);
    draw_rect(x, y + height - thickness, width, thickness, color);
    draw_rect(x, y, thickness, height, color);
    draw_rect(x + width - thickness, y, thickness, height, color);
}

void clear_screen(unsigned int color) {
    draw_rect(0, 0, fb_width, fb_height, color);
}

void graphics_draw_char(int x, int y, char c, unsigned int color, int scale) {
    int row;
    int col;
    unsigned char bits;

    if (c == ' ') {
        return;
    }

    if (scale < 1) {
        scale = 1;
    }

    for (row = 0; row < 7; row++) {
        bits = glyph_rows(c, row);

        for (col = 0; col < 5; col++) {
            if (bits & (1 << (4 - col))) {
                draw_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

void graphics_draw_text(int x, int y, const char* text, unsigned int color, int scale) {
    int i;
    int step;

    if (scale < 1) {
        scale = 1;
    }

    step = 6 * scale;
    i = 0;

    while (text[i] != '\0') {
        graphics_draw_char(x + i * step, y, text[i], color, scale);
        i++;
    }
}
