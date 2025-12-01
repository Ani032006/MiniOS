// kernel/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

// For now, get_char() uses polled I/O (no interrupts needed)
char get_char(void);

// Stub for future (IRQ path); safe to call even if unused
void keyboard_init(void);

#endif

