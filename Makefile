# ==========================
# MiniOS Makefile
# ==========================

CROSS = i686-elf-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
NASM = nasm

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32
LDFLAGS = -T linker.ld -nostdlib

ISO = MiniOS.iso

OBJS = \
boot/boot.o \
kernel/kernel.o \
kernel/keyboard.o \
kernel/screen.o \
kernel/idt.o \
kernel/interrupts.o \
kernel/string.o

# Default build
all: $(ISO)

# Assemble bootloader
boot/boot.o: boot/boot.s
	$(AS) boot/boot.s -o boot/boot.o

# Kernel C files
kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Keyboard interrupt asm file
kernel/interrupts.o: kernel/interrupts.asm
	$(NASM) -f elf32 kernel/interrupts.asm -o kernel/interrupts.o

# Link kernel
MiniOS.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o MiniOS.bin $(OBJS)

# Create ISO directory
iso_dir:
	mkdir -p isodir/boot/grub
	cp MiniOS.bin isodir/boot/kernel.bin
	echo 'set timeout=0' > isodir/boot/grub/grub.cfg
	echo 'set default=0' >> isodir/boot/grub/grub.cfg
	echo 'menuentry "MiniOS" { multiboot /boot/kernel.bin }' >> isodir/boot/grub/grub.cfg

# Build GRUB ISO
$(ISO): MiniOS.bin iso_dir
	i686-elf-grub-mkrescue -o $(ISO) isodir

# Run in QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -boot d -vga std -serial stdio

# Clean build
clean:
	rm -f boot/*.o kernel/*.o
	rm -f MiniOS.bin MiniOS.iso
	rm -rf isodir

.PHONY: all clean run iso_dir

