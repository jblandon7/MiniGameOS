#ifndef VGA_H
#define VGA_H

void vga_clear(void);			// Clear the screen
void vga_print(const char* str);	// Print text to the screen
void vga_put_char(char c);		// Print one character
void vga_print_int(int num);		//Print one number at a time 

#endif 
