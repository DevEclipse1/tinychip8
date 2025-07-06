# Tiny-Chip8
A minimal, efficient Chip-8 emulator for embedded systems.

## Features
- Basic CHIP-8 CPU emulation (display features included)
- Simple and lightweight design
- Includes built-in CHIP-8 font set
- Easy to integrate into your projects

## Usage
Initialize the emulator, load your CHIP-8 program into memory starting at 0x200, then repeatedly call the cycle function to execute instructions.

```c
tinychip8 emulator;

void initialize_tc8()
{
  tinychip8_bind(&emulator);
  emulator.init(&emulator);
}

while (running) {
  emulator.cycle(&emulator);
}

...
