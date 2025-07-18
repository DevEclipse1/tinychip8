#include <stdio.h>
#include "../../src/tinychip8.h"

tinychip8 tc8;

// maze demo from https://github.com/kripod/chip8-roms/tree/master
const unsigned char rom[] = {
  0x60, 0x0A, 0x65, 0x05, 0x66, 0x0A, 0x67, 0x0F, 0x68, 0x14,
  0x61, 0x01, 0x62, 0x01, 0x63, 0x01, 0x64, 0x01, 0x60, 0x0A,
  0xA2, 0x78, 0xD0, 0x56, 0x70, 0x0A, 0xA2, 0x7E, 0xD0, 0x66,
  0x70, 0x0A, 0xA2, 0x84, 0xD0, 0x76, 0x70, 0x0A, 0xA2, 0x8A,
  0xD0, 0x86, 0x6A, 0x03, 0xFA, 0x15, 0x60, 0x0A, 0xA2, 0x78,
  0xD0, 0x56, 0x45, 0x14, 0x61, 0xFF, 0x45, 0x01, 0x61, 0x01,
  0x85, 0x14, 0xD0, 0x56, 0x70, 0x0A, 0xA2, 0x7E, 0xD0, 0x66,
  0x46, 0x14, 0x62, 0xFF, 0x46, 0x01, 0x62, 0x01, 0x86, 0x24,
  0xD0, 0x66, 0x70, 0x0A, 0xA2, 0x84, 0xD0, 0x76, 0x47, 0x14,
  0x63, 0xFF, 0x47, 0x01, 0x63, 0x01, 0x87, 0x34, 0xD0, 0x76,
  0x70, 0x0A, 0xA2, 0x8A, 0xD0, 0x86, 0x48, 0x14, 0x64, 0xFF,
  0x48, 0x01, 0x64, 0x01, 0x88, 0x44, 0xD0, 0x86, 0x12, 0x2A,
  0xFF, 0x03, 0x0C, 0x30, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFC,
  0xC0, 0xFF, 0xF0, 0xCC, 0xCC, 0xF0, 0xCC, 0xC3, 0x3C, 0xC3,
  0xC3, 0xC3, 0xC3, 0x3C
};
size_t rombytes = sizeof(rom) / sizeof(rom[0]);

void myclearscreen(struct tinychip8* self)
{
    printf("\e[1;1H\e[2J");
}

void myplotpixel(struct tinychip8* self, tc8_8bit x, tc8_8bit y)
{
    printf("\033[%d;%dH#", y + 1, x + 1);
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    // setting the seed is important if you want it to always be (pseudo)random
    srand(time(NULL));

    // bind all the functions, unless nothing will work
    tinychip8_bind(&tc8);
    tc8.init(&tc8);

    // load the font
    for (int i = 0; i < 80; ++i)
      tc8.memory[0x050 + i] = tinychip8_font[i];
  
    // load the ROM, MUST BE AT ADDRESS 0x200
    for (int i = 0; i < rombytes; ++i) {
        tc8.memory[0x200 + i] = rom[i];
    }

    // these 2 functions are important if you actually want to draw something on the screen
    tc8.clear_screen = myclearscreen;
    tc8.plot_pixel = myplotpixel;

    // run it all in a loop, you can always check if the rom is at the end, but for this example, this works
    while (1)
    {
        tc8.clear_screen(&tc8);
        tc8.cycle(&tc8); // cycle is very important, unless it will just be stuck at the beginning and wont execute anything
    }

    return 0;
}
