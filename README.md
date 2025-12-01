cat > README.md << 'EOF'
# MiniOS: A Lightweight Educational Operating System

MiniOS is a minimal operating system built from scratch using C and Assembly, 
designed as an **educational project** to explore kernel development, 
interrupt handling, keyboard input, and simple shell design.

## ✨ Features

- Custom bootloader (Multiboot compliant)
- 32-bit protected mode kernel
- VGA text-mode driver (direct writes to 0xB8000)
- Keyboard input handling (IRQ1, interrupt-driven)
- Simple embedded shell with commands:
  - `help`, `about`, `clear`, `date`, `sysinfo`
  - `mkdir`, `cd`, `ls`, `echo`
  - `calc` (basic arithmetic)
  - `motivate`, `credits`, `halt`
- Two modes:
  - **Auto Demo Mode** – runs and explains all commands
  - **Interactive Mode** – user can type and execute commands

## 🛠 Toolchain & Requirements

- Cross compiler: `i686-elf-gcc`
- Assembler: `nasm`
- Linker: `i686-elf-ld`
- Emulator: `qemu-system-i386`
- ISO tools: `i686-elf-grub-mkrescue`, `xorriso`

## 🚀 Build & Run

```bash
make clean
make
make run
