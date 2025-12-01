// kernel/kernel.c — MiniOS v4.0 (Educational + Keyboard + Backspace support)

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "screen.h"     // putchar, print, clear_screen, print_backspace
#include "keyboard.h"   // get_char, keyboard_init
#include "idt.h"        // idt_init
#include "string.h"     // your kernel string.h: strcmp, strncmp, strlen

// ------------------------------------------------------------
// Small helpers (no libc)
// ------------------------------------------------------------
static int to_int(const char* s) {
    int sign = 1, i = 0, val = 0;
    if (s[0] == '-') { sign = -1; i = 1; }
    for (; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') break;
        val = val * 10 + (s[i] - '0');
    }
    return sign * val;
}

static void print_int(int v) {
    char buf[16];
    int i = 0, neg = 0;

    if (v == 0) { putchar('0'); return; }
    if (v < 0) { neg = 1; v = -v; }

    while (v && i < 15) { buf[i++] = (char)('0' + (v % 10)); v /= 10; }
    if (neg) buf[i++] = '-';
    while (i--) putchar(buf[i]);
}

// ------------------------------------------------------------
// Line input with backspace editing
// ------------------------------------------------------------
#define INPUT_MAX 128
static char input_buf[INPUT_MAX];

static void get_line(char* out, int out_sz) {
    int len = 0;
    while (1) {
        char c = get_char();
        if (c == '\n' || c == '\r') {
            out[len] = '\0';
            print("\n");
            return;
        } else if (c == '\b') {
            if (len > 0) {
                len--;
                print_backspace(); // erase last character on screen
            }
        } else if (c >= 32 && c < 127) { // printable ASCII
            if (len < out_sz - 1) {
                out[len++] = c;
                putchar(c);
            }
        }
        // ignore others (e.g., modifiers)
    }
}

// ------------------------------------------------------------
// Prompt
// ------------------------------------------------------------
static void prompt() {
    print("\nMiniOS $ ");
}

// ------------------------------------------------------------
// Commands (each explains itself first, then shows output)
// ------------------------------------------------------------
static void cmd_help() {
    print("\n[HELP]\n");
    print("The 'help' command lists all available commands and their purpose.\n");
    print("Available:\n");
    print("  help, about, clear, echo <msg>, sum, calc, date, sysinfo,\n");
    print("  mkdir <d>, cd <d>, ls, motivate, credits, halt\n");
}

static void cmd_about() {
    print("\n[ABOUT]\n");
    print("Shows MiniOS information. Demonstrates how kernels display metadata.\n");
    print("MiniOS v4.0 — an educational hobby kernel by Aniruddh V K.\n");
}

static void cmd_clear() {
    print("\n[CLEAR]\n");
    print("Clears the VGA text buffer and resets the cursor to the top-left.\n");
    clear_screen();
}

static void cmd_echo(const char* msg) {
    print("\n[ECHO]\n");
    print("Echo repeats your input to demonstrate basic I/O handling.\n");
    print("Output: ");
    print(msg);
    print("\n");
}

static void cmd_sum() {
    char a[16], b[16];
    print("\n[SUM]\n");
    print("Adds two integers. Demonstrates simple numeric input parsing.\n");
    print("Enter first number: ");
    get_line(a, sizeof(a));
    print("Enter second number: ");
    get_line(b, sizeof(b));
    int res = to_int(a) + to_int(b);
    print("Result: ");
    print_int(res);
    print("\n");
}

static void cmd_calc() {
    char s1[16], s2[16];
    print("\n[CALC]\n");
    print("Performs +, -, *, / on two integers. Demonstrates parsing and branching.\n");
    print("Enter first number: ");
    get_line(s1, sizeof(s1));
    print("Enter operator (+ - * /): ");
    char op = get_char(); putchar(op); print("\n");
    print("Enter second number: ");
    get_line(s2, sizeof(s2));

    int a = to_int(s1), b = to_int(s2), r = 0;
    if (op == '+') r = a + b;
    else if (op == '-') r = a - b;
    else if (op == '*') r = a * b;
    else if (op == '/') { if (b != 0) r = a / b; else { print("Error: divide by zero\n"); return; } }
    else { print("Invalid operator\n"); return; }

    print("Result: "); print_int(r); print("\n");
}

static void cmd_date() {
    print("\n[DATE]\n");
    print("Simulated date/time. A real OS reads RTC/HPET/ACPI for timekeeping.\n");
    print("Current: 10-Nov-2025 9:30 IST\n");
}

static void cmd_sysinfo() {
    print("\n[SYSINFO]\n");
    print("Shows static kernel/system info to illustrate OS introspection.\n");
    print("CPU: i686 (sim)\nRAM: 512MB (virt)\nKernel: MiniOS v4.0\n");
}

static void cmd_mkdir(const char* d) {
    print("\n[MKDIR]\n");
    print("Creates a directory (simulated; no real FS yet).\n");
    if (!d || d[0] == '\0') { print("Usage: mkdir <name>\n"); return; }
    print("Created: ");
    print(d);
    print("\n");
}

static void cmd_cd(const char* d) {
    print("\n[CD]\n");
    print("Changes current directory (simulated path variable, no real FS).\n");
    if (!d || d[0] == '\0') { print("Usage: cd <dir>\n"); return; }
    print("Now in: ");
    print(d);
    print("\n");
}

static void cmd_ls() {
    print("\n[LS]\n");
    print("Lists current directory (simulated entries).\n");
    print("bin/ boot/ home/ kernel/ user/\n");
}

static void cmd_motivate() {
    print("\n[MOTIVATE]\n");
    print("Displays a motivational line to encourage steady OS learning.\n");
    print("\"A great OS isn’t built overnight it’s crafted one syscall at a time.\"\n");
}

static void cmd_credits() {
    print("\n[CREDITS]\n");
    print("Acknowledges authors/contributors.\n");
    print("MiniOS created by: Aniruddh V K\n");
}

static void cmd_halt() {
    print("\n[HALT]\n");
    print("Stops the CPU (in a real OS, ACPI power-off or halt loop).\n");
    for(;;) { /* hlt could be used here if desired */ }
}

// ------------------------------------------------------------
// Auto-Demo Mode (runs each command with examples)
// ------------------------------------------------------------
static void auto_demo() {
    print("\n=== AUTO DEMO MODE ===\n");
    cmd_help();
    cmd_about();
    cmd_sysinfo();
    cmd_date();
    cmd_echo("Hello MiniOS!");
    cmd_sum();
    cmd_calc();
    cmd_mkdir("projects");
    cmd_cd("projects");
    cmd_ls();
    cmd_motivate();
    cmd_credits();
    print("=== DEMO COMPLETE switching to interactive ===\n");
}

// ------------------------------------------------------------
// Interactive Shell
// ------------------------------------------------------------
static void interactive_shell() {
    while (1) {
        prompt();
        get_line(input_buf, INPUT_MAX);

        if (!strcmp(input_buf, "help")) cmd_help();
        else if (!strcmp(input_buf, "about")) cmd_about();
        else if (!strcmp(input_buf, "clear")) cmd_clear();
        else if (!strcmp(input_buf, "sum")) cmd_sum();
        else if (!strcmp(input_buf, "calc")) cmd_calc();
        else if (!strcmp(input_buf, "date")) cmd_date();
        else if (!strcmp(input_buf, "sysinfo")) cmd_sysinfo();
        else if (!strcmp(input_buf, "ls")) cmd_ls();
        else if (!strcmp(input_buf, "motivate")) cmd_motivate();
        else if (!strcmp(input_buf, "credits")) cmd_credits();
        else if (!strcmp(input_buf, "halt")) cmd_halt();
        else if (!strncmp(input_buf, "echo ", 5)) cmd_echo(input_buf + 5);
        else if (!strncmp(input_buf, "mkdir ", 6)) cmd_mkdir(input_buf + 6);
        else if (!strncmp(input_buf, "cd ", 3)) cmd_cd(input_buf + 3);
        else print("Unknown command. Try 'help'\n");
    }
}

// ------------------------------------------------------------
// Kernel entry
// ------------------------------------------------------------
void kernel_main() {
    clear_screen();
    print("Welcome to MiniOS v4.0 — The Educational Operating System!\n");
    print("Choose mode:\n  1) Auto Demo\n  2) Interactive\n> ");

    // Initialize IDT + PIC + Keyboard, then enable interrupts
   // idt_init();
    //keyboard_init();
    //__asm__ __volatile__("sti");

    // Read choice (single key)
    char choice = get_char();
    putchar(choice);
    print("\n");

    if (choice == '1') {
        auto_demo();
    }
    // fallthrough to interactive
    interactive_shell();
}

