// kernel/keyboard.c — polled PS/2 keyboard (works without IDT/IRQs)

#include <stdint.h>
#include "port_io.h"
#include "keyboard.h"

// Minimal scancode -> ASCII (US layout, lowercase)
static const char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // 0x0E Backspace
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',      // 0x1C Enter
    0,  'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,  ' ', 0, // the rest can be zero
};

// simple delay
static inline void io_wait(void) { outb(0x80, 0); }

// Poll controller status (bit0 = output buffer full)
static inline int kb_has_data(void) {
    return (inb(0x64) & 0x01) != 0;
}

// Return one ASCII char, blocking until a key is pressed.
// Handles Enter -> '\n', Backspace -> '\b'. Ignores key releases (0x80 bit).
char get_char(void) {
    while (1) {
        // wait until there is data
        while (!kb_has_data()) { io_wait(); }

        uint8_t sc = inb(0x60);

        // ignore key releases (high bit set)
        if (sc & 0x80) continue;

        if (sc < 128) {
            if (sc == 0x1C) return '\n'; // Enter
            if (sc == 0x0E) return '\b'; // Backspace

            char ch = scancode_map[sc];
            if (ch) return ch;
        }
        // If unmapped, keep looping
    }
}

// Stub for future IRQ mode; safe to call now
void keyboard_init(void) { /* no-op in polled mode */ }
// dummy interrupt handler for linker satisfaction
void keyboard_handler_main(void) { }
