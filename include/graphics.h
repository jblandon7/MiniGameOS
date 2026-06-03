#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GFX_BLACK 0x050506
#define GFX_CHARCOAL 0x0A0C0D
#define GFX_ARMOR_GRAY 0x1B1F21
#define GFX_SMOKY_GRAY 0x5F4B53
#define GFX_DEEP_RED 0x702029
#define GFX_BLOOD_RED 0x591F25
#define GFX_BURGUNDY 0x471114
#define GFX_LIGHT_TEXT 0xD8D8D8
#define GFX_WHITE 0xFFFFFF

int graphics_init(unsigned int multiboot_magic, unsigned int multiboot_info_addr);
int graphics_is_available(void);
int graphics_width(void);
int graphics_height(void);

void draw_pixel(int x, int y, unsigned int color);
void draw_rect(int x, int y, int width, int height, unsigned int color);
void draw_border_rect(int x, int y, int width, int height, int thickness, unsigned int color);
void clear_screen(unsigned int color);

void graphics_draw_char(int x, int y, char c, unsigned int color, int scale);
void graphics_draw_text(int x, int y, const char* text, unsigned int color, int scale);

#endif
