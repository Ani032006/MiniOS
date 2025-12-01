#include "screen.h"
#include "port_io.h"

#define VGA_ADDRESS 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

static unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;
static int cursor_row = 0;
static int cursor_col = 0;

static unsigned short vga_entry(unsigned char ch, unsigned char color) {
    return (unsigned short)ch | (unsigned short)color << 8;
}

static void move_cursor() {
    unsigned short pos = cursor_row * VGA_COLS + cursor_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void clear_screen() {
    unsigned short blank = vga_entry(' ', 0x07);
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        vga_buffer[i] = blank;
    }
    cursor_row = 0;
    cursor_col = 0;
    move_cursor();
}

void putchar(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        vga_buffer[cursor_row * VGA_COLS + cursor_col] = vga_entry(c, 0x07);
        cursor_col++;
        if (cursor_col >= VGA_COLS) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    if (cursor_row >= VGA_ROWS) {
        clear_screen();
    }
    move_cursor();
}

void print(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}
// Backspace support using cursor_row and cursor_col
void print_backspace() {
    if (cursor_col > 0) {
        cursor_col--;
    } else {
        if (cursor_row > 0) {
            cursor_row--;
            cursor_col = 79; // end of previous line
        }
    }

    // Erase the char visually
    putchar(' ');

    // Move cursor back again
    if (cursor_col > 0) {
        cursor_col--;
    }
}

