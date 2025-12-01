// kernel/idt.c — Interrupt Descriptor Table setup with keyboard IRQ

#include <stdint.h>
#include "port_io.h"


// === IDT structures ===
#define IDT_ENTRIES 256

struct IDTEntry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct IDTEntry idt[IDT_ENTRIES];
struct IDTPointer idt_ptr;

// === External keyboard handler (from interrupts.asm) ===
extern void keyboard_handler();

// === Inline assembly: lidt ===
static inline void lidt(void* base, uint16_t size) {
    struct {
        uint16_t length;
        void* base;
    } __attribute__((packed)) IDTR = { size, base };

    __asm__ volatile ("lidt (%0)" :: "r" (&IDTR));
}

// === Set one entry in IDT ===
void idt_set_gate(int n, uint32_t handler) {
    idt[n].base_low = handler & 0xFFFF;
    idt[n].sel = 0x08;         // Kernel code segment selector
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;       // Present, ring 0, 32-bit interrupt gate
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

// === PIC Remapping ===
void pic_remap() {
    // ICW1: start initialization
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2: remap offset
    outb(0x21, 0x20); // Master PIC vector offset 0x20
    outb(0xA1, 0x28); // Slave PIC vector offset 0x28

    // ICW3: tell master/slave relationship
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // ICW4: 8086/88 mode
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Clear masks
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

// === Initialize IDT with keyboard handler ===
void idt_init() {
    pic_remap(); // Remap IRQs

    idt_ptr.limit = sizeof(struct IDTEntry) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Clear all entries
    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0);

    // IRQ1 (keyboard) -> interrupt 33 (0x21)
    idt_set_gate(0x21, (uint32_t)keyboard_handler);

    // Load IDT
    lidt(&idt, sizeof(struct IDTEntry) * IDT_ENTRIES - 1);
}

