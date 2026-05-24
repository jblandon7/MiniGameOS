CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld

OBJ = boot/kernel_entry.o boot/interrupts.o kernel/kernel.o kernel/vga.o kernel/idt.o kernel/pic.o kernel/timer.o kernel/game.o kernel/keyboard.o

all: os.iso

boot/kernel_entry.o: boot/kernel_entry.asm
	$(AS) -f elf32 boot/kernel_entry.asm -o boot/kernel_entry.o

boot/interrupts.o: boot/interrupts.asm
	$(AS) -f elf32 boot/interrupts.asm -o boot/interrupts.o

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel/kernel.o

kernel/vga.o: kernel/vga.c
	$(CC) $(CFLAGS) -c kernel/vga.c -o kernel/vga.o

kernel/idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c kernel/idt.c -o kernel/idt.o

kernel/pic.o: kernel/pic.c
	$(CC) $(CFLAGS) -c kernel/pic.c -o kernel/pic.o

kernel/timer.o: kernel/timer.c
	$(CC) $(CFLAGS) -c kernel/timer.c -o kernel/timer.o

kernel/game.o: kernel/game.c
	$(CC) $(CFLAGS) -c kernel/game.c -o kernel/game.o

kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o kernel/keyboard.o

kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o kernel.bin

os.iso: kernel.bin grub/grub.cfg
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cp grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o os.iso iso

run: os.iso
	qemu-system-i386 -boot d -cdrom os.iso -display curses -no-reboot -no-shutdown
clean:
	rm -rf *.iso *.bin iso
	rm -f boot/*.o kernel/*.o
