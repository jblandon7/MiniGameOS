#include "vga.h"
#define VGA_WIDTH 80							// VGA_WIDTH has a max column of 80
#define VGA_HEIGHT 25							// VGA_COLUMN has a max row of 25
#define VGA_MEMORY ((volatile unsigned short*)0xB8000)			// Points to memory address, positive 16 bit (2 bytes spaces)

static int row = 0;							// index for row
static int col = 0;							// index for column
static unsigned char color = 0x0F;					// 4 bits for the foreground and 4 bits for the background (1 byte)

// Combine word and color to make 2 byte VGA entry
static unsigned short make_vga_entry(char c, unsigned char color) {
    return (unsigned short)c | ((unsigned short)color << 8);		// Shift the color 8 bits left and then or it with the top 8 bits of the chacter ( Combine bits)
}

// Clears the VGA text screen, uses nested loop but because its a fixed size O(1) complexity
void vga_clear(void) {
    int r;								// Row variable
    int c;								// Column variable

    for (r = 0; r < VGA_HEIGHT; r++) {					// Start at the top left
        for (c = 0; c < VGA_WIDTH; c++) {
            VGA_MEMORY[r * VGA_WIDTH + c] = make_vga_entry(' ', color);	// Put a blank space there using the current color
        }
    }

    row = 0;								// Reinitalize the row and col to 0
    col = 0;
}

// Function writes one character at a time
void vga_put_char(char c) {
    if (c == '\n') {							// IF character is newline start a new row and the first index
        row++;
        col = 0;
        return;
    }

    VGA_MEMORY[row * VGA_WIDTH + col] = make_vga_entry(c, color);	// At this index write this character and this color
    col++;								// Move forward one

    if (col >= VGA_WIDTH) {						// If column location bigger than width move to the next row (line is full move to the next line)
        col = 0;
        row++;
    }

    if (row >= VGA_HEIGHT) {						// Start right back at the top if used all of height
        row = 0;
    }
}

// The print function
void vga_print(const char* str) {
    int i = 0;								// index variable

    while (str[i] != '\0') {						// if the index is not null then print character and go to the next one
        vga_put_char(str[i]);
        i++;
    }
}

// Function to print one integer
void vga_print_int(int num) {
    char buffer[12];			// Buffer that holds 12 Characters
    int i = 0;
    int j;

    if (num == 0) {			// If Num is 0 then put 0
        vga_put_char('0');
        return;
    }

    if (num < 0) {			// If negative print the '-' first
        vga_put_char('-');
        num = -num;			// Remove the negative
    }

    while (num > 0) {
        buffer[i] = '0' + (num % 10);	// Remove one digit at a time then make char
        num = num / 10;			// Extract the last digit
        i++;				// Go to the next place in buffer
    }

    for (j = i - 1; j >= 0; j--) {	//  Prints the buffer backwords
        vga_put_char(buffer[j]);
    }
}
