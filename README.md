# MiniGameOS

MiniGameOS is a bare-metal x86 mini operating system written in C and x86 assembly. It boots with GRUB, runs inside QEMU, and runs a simple 2D text-mode game.

## Features

- GRUB bootloader support
- Freestanding C kernel
- VGA text-mode rendering
- Keyboard input through x86 I/O ports
- 2D game loop
- Player movement with WASD
- Enemy collision system
- Partial screen redraw to reduce flicker

## Build and Run

```bash
make clean
make
make run
