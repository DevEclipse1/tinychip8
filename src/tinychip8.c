#include "tinychip8.h"

tc8_8bit tinychip8_font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void tinychip8_init(struct tinychip8* self) {
    self->pc = 0x200;
    self->i = 0;
    self->sp = 0;
    self->current_state = TC8_LOAD;

    for (int i = 0; i < 0x1000; i++) self->memory[i] = 0;
    for (int i = 0; i < 0xf; i++) self->v[i] = 0;
    for (int i = 0; i < 16; i++) self->stack[i] = 0;
    for (int i = 0; i < 16; i++) self->keys[i] = tc8_false;

    for (int i = 0; i < 80; ++i)
        self->memory[0x050 + i] = tinychip8_font[i];
}

void tinychip8_execute(struct tinychip8* self, const tc8_16bit op)
{
    self->pc += 0x2;

    tc8_16bit fn = op & 0xF000;
    tc8_16bit nnn = op & 0x0FFF;
    tc8_8bit nn = op & 0x00FF;
    tc8_8bit n = op & 0x000F;
    tc8_8bit x = (op & 0x0F00) >> 8;
    tc8_8bit y = (op & 0x00F0) >> 4;

    if (op == 0x00E0)
    {
        if (self->clear_screen)
        {
            self->clear_screen(self);
        }
        memset(self->display, 0, sizeof(self->display));
    }
    else if (op == 0x00EE)
    {
        --self->sp;
        self->pc = self->stack[self->sp];
    }
    else if (fn == 0x1000)
    {
        self->pc = nnn;
    }
    else if (fn == 0x2000)
    {
        self->stack[self->sp] = self->pc;
        ++self->sp;
        self->pc = nnn;
    }
    else if (fn == 0x3000)
    {
        if (self->v[x] == nn)
            self->pc += 0x2;
    }
    else if (fn == 0x4000)
    {
        if (self->v[x] != nn)
            self->pc += 0x2;
    }
    else if (fn == 0x5000 && n == 0)
    {
        if (self->v[x] == self->v[y])
            self->pc += 0x2;
    }
    else if (fn == 0x6000)
    {
        self->v[x] = nn; 
    }
    else if (fn == 0x7000)
    {
        self->v[x] += nn;
    }
    else if (fn == 0x8000 && n == 0x0000)
    {
        self->v[x] = self->v[y];
    }
    else if (fn == 0x8000 && n == 0x0001)
    {
        self->v[x] |= self->v[y];
    }
    else if (fn == 0x8000 && n == 0x0002)
    {
        self->v[x] &= self->v[y];
    }
    else if (fn == 0x8000 && n == 0x0003)
    {
        self->v[x] ^= self->v[y];
    }
    else if (fn == 0x8000 && n == 0x0004)
    {
        tc8_16bit sum = self->v[x] + self->v[y];
        self->v[0xF] = sum > 0xFF;
        self->v[x] = sum & 0xFF;
    }
    else if (fn == 0x8000 && n == 0x0005)
    {
        self->v[0xF] = self->v[x] >= self->v[y];
        self->v[x] -= self->v[y];
    }
    else if (fn == 0x8000 && n == 0x0006)
    {
        self->v[0xF] = self->v[x] & 1;
        self->v[x] >>= 1;
    }
    else if (fn == 0x8000 && n == 0x0007)
    {
        self->v[0xF] = self->v[y] >= self->v[x];
        self->v[x] = self->v[y] - self->v[x];
    }
    else if (fn == 0x8000 && n == 0x000E)
    {
        self->v[0xF] = (self->v[x] & 0x80) >> 7;
        self->v[x] <<= 1;
    }
    else if (fn == 0x9000 && n == 0)
    {
        if (self->v[x] != self->v[y])
            self->pc += 0x2;
    }
    else if (fn == 0xA000)
    {
        self->i = nnn;
    }
    else if (fn == 0xB000)
    {
        self->pc = nnn + self->v[0];
    }
    else if (fn == 0xC000)
    {
        self->v[x] = (rand() % 256) & nn;
    }
    else if (fn == 0xD000)
    {
        self->v[0xf] = 0;
        for (int height = 0; height < n; ++height)
        {
            tc8_8bit sprite = self->memory[self->i + height];
            for (int width = 0; width < 8; ++width)
            {
                tc8_8bit pixel = (sprite >> (7 - width)) & 1;
                if (pixel == 1)
                {
                    tc8_8bit xcoord_base = self->v[x];
                    tc8_8bit ycoord_base = self->v[y];

                    tc8_8bit xcoord = (xcoord_base + width) % 64;
                    tc8_8bit ycoord = (ycoord_base + height) % 32;
                    int idx = ycoord * 64 + xcoord;
                    if (self->display[idx] == 1)
                        self->v[0xf] = 1;
                    self->display[idx] ^= 1;
                    
                    if (self->plot_pixel)
                    {
                        self->plot_pixel(self, xcoord, ycoord);
                    }
                }
            }
        }
    }
    else if ((op & 0xF0FF) == 0xF007)
    {
        self->v[x] = self->dt;
    }
    else if (fn == 0xF000 && n == 0x000A) {
        tc8_bool key_pressed = tc8_false;
        for (int k = 0; k < 16; ++k) {
            if (self->keys[k]) {
                self->v[x] = k;
                key_pressed = tc8_true;
                break;
            }
        }
        if (!key_pressed) {
            self->pc -= 2;
        }
    }
    else if ((op & 0xF0FF) == 0xF015)
    {
        self->dt = self->v[x];
    }
    else if ((op & 0xF0FF) == 0xF018)
    {
        self->st = self->v[x];
    }
    else if ((op & 0xF0FF) == 0xF01E)
    {
        self->i += self->v[x];
    }
    else if ((op & 0xF0FF) == 0xF029)
    {
        self->i = 0x050 + (self->v[x] & 0x0F) * 5;
    }
    else if ((op & 0xF0FF) == 0xF033)
    {
        self->memory[self->i]     = self->v[x] / 100;
        self->memory[self->i + 1] = (self->v[x] / 10) % 10;
        self->memory[self->i + 2] = self->v[x] % 10;
    }
    else if ((op & 0xF0FF) == 0xF055)
    {
        for (int j = 0; j <= x; ++j)
            self->memory[self->i + j] = self->v[j];
    }
    else if ((op & 0xF0FF) == 0xF065)
    {
        for (int j = 0; j <= x; ++j)
            self->v[j] = self->memory[self->i + j];
    }
}

void tinychip8_cycle(struct tinychip8* self)
{
    const tc8_16bit op = (self->memory[self->pc] << 8) | self->memory[self->pc + 1];
    self->execute(self, op);
    
    self->current_state = TC8_RUN;
}

void tinychip8_bind(struct tinychip8* c)
{
    c->init = tinychip8_init;
    c->cycle = tinychip8_cycle;
    c->execute = tinychip8_execute;
}