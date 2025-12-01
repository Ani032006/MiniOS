#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "string.h"


#include "screen.h"
#include "keyboard.h"
#include "idt.h"

// ==== INPUT BUFFER ====
#define INPUT_BUFFER 128
char input[INPUT_BUFFER];

// Forward declarations
void auto_demo();
void interactive_shell();

// ==== Utility: Get line from keyboard ====
void get_line(char *buf) {
    int len = 0;
    while (1) {
        char c = get_char();
        if (c == '\n') { buf[len] = 0; print("\n"); return; }
        if (c == '\b' && len > 0) {
            len--; print_backspace();
        } else if (c >= ' ' && len < INPUT_BUFFER-1) {
            buf[len++] = c;
            putchar(c);
        }
    }
}

// ==== COMMANDS ====

void cmd_help() {
    print("\n[HELP COMMAND EXECUTED]\n");
    print("This command lists all commands and explains their purpose.\n");
    print("Available Commands:\n");
    print(" help       - List all commands\n");
    print(" about      - Know what MiniOS is\n");
    print(" clear      - Clear the screen\n");
    print(" echo       - Repeat input\n");
    print(" sum        - Add two numbers\n");
    print(" calc       - Full calculator (+ - * /)\n");
    print(" date       - Show simulated date/time\n");
    print(" sysinfo    - Show OS info\n");
    print(" mkdir      - Create folder (simulated)\n");
    print(" cd         - Change directory (simulated)\n");
    print(" ls         - List directory (simulated)\n");
    print(" motivate   - Show motivational message\n");
    print(" credits    - Show creator details\n");
    print(" halt       - Stop CPU\n");
}

void cmd_about() {
    print("\n[ABOUT COMMAND EXECUTED]\n");
    print("MiniOS is an educational OS created by Aniruddh V K.\n");
    print("It demonstrates kernel-level programming, interrupts, input system, and shell logic.\n");
}

void cmd_clear() {
    clear_screen();
    print("Screen cleared! (VGA buffer reset)\n");
}

void cmd_echo(char *txt) {
    print("\n[ECHO COMMAND]\n");
    print("This prints back whatever you type.\n");
    print("Output: "); print(txt); print("\n");
}

void cmd_sum() {
    char a[8], b[8];
    print("Enter first number: "); get_line(a);
    print("Enter second number: "); get_line(b);
    int r = (a[0]-'0') + (b[0]-'0');
    print("Result: "); putchar(r + '0'); print("\n");
}

void cmd_calc() {
    char x[8], y[8]; char op;
    print("Enter first number: "); get_line(x);
    print("Enter operator (+ - * /): "); op = get_char(); putchar(op); print("\n");
    print("Enter second number: "); get_line(y);

    int a = x[0]-'0', b = y[0]-'0', r = 0;

    if (op == '+') r = a + b;
    else if (op == '-') r = a - b;
    else if (op == '*') r = a * b;
    else if (op == '/' && b != 0) r = a / b;
    else { print("Invalid!\n"); return; }

    print("Result = "); putchar(r + '0'); print("\n");
}

void cmd_date() {
    print("\n[DATE]\nSimulated: 04-Nov-2025 18:30 IST\n");
}

void cmd_sysinfo() {
    print("\n[SYSINFO]\nCPU: x86 | RAM: 512MB | Kernel: MiniOS v4.0 | INT+I/O Enabled\n");
}

void cmd_mkdir(char *d) {
    print("\n[MKDIR] Created folder: "); print(d); print("\n");
}

void cmd_cd(char *d) {
    print("\n[CD] Directory changed to: "); print(d); print("\n");
}

void cmd_ls() {
    print("\n[LS] Showing simulated files:\n");
    print(" boot/ kernel/ home/ bin/\n kernel.bin README.txt\n");
}

void cmd_motivate() {
    print("\n[MOTIVATE]\nRemember: Great OS devs were beginners once too!\n");
}

void cmd_credits() {
    print("\n[CREDITS]\nMiniOS developed by Aniruddh V K.\n");
}

void cmd_halt() {
    print("\nSystem halting...\n");
    for(;;);
}

// ==== AUTO DEMO ====

void auto_demo() {
    print("\n=== AUTO DEMO START ===\n");
    cmd_help(); cmd_about(); cmd_sysinfo(); cmd_date();
    cmd_mkdir("projects"); cmd_cd("projects"); cmd_ls();
    cmd_echo("Hello OS World!"); cmd_sum(); cmd_calc();
    cmd_motivate(); cmd_credits();
    print("=== DEMO END — Switching to interactive ===\n");
}

// ==== INTERACTIVE SHELL ====

void interactive_shell() {
    while (1) {
        print("\nMiniOS $ ");
        get_line(input);

        if (!strcmp(input,"help")) cmd_help();
        else if (!strcmp(input,"about")) cmd_about();
        else if (!strcmp(input,"clear")) cmd_clear();
        else if (!strcmp(input,"sum")) cmd_sum();
        else if (!strcmp(input,"calc")) cmd_calc();
        else if (!strcmp(input,"date")) cmd_date();
        else if (!strcmp(input,"sysinfo")) cmd_sysinfo();
        else if (!strncmp(input,"mkdir ",6)) cmd_mkdir(input+6);
        else if (!strncmp(input,"cd ",3)) cmd_cd(input+3);
        else if (!strcmp(input,"ls")) cmd_ls();
        else if (!strncmp(input,"echo ",5)) cmd_echo(input+5);
        else if (!strcmp(input,"motivate")) cmd_motivate();
        else if (!strcmp(input,"credits")) cmd_credits();
        else if (!strcmp(input,"halt")) cmd_halt();
        else print("Unknown command. Try 'help'\n");
    }
}

// ==== KERNEL ENTRY ====

void kernel_main() {
    clear_screen();
    print("Welcome to MiniOS v4.0 — The Educational Operating System!\n");
    print("Choose mode:\n1. Auto Demo\n2. Interactive\n> ");

    idt_init();       // load IDT + PIC
    keyboard_init();  // enable keyboard
    __asm__ volatile("sti"); // enable interrupts

    char c = get_char();
    putchar(c); print("\n");

    if (c == '1') auto_demo();
    interactive_shell();
}

