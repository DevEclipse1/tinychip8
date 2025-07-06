#ifndef TC8
#define TC8

#define tc8_8bit unsigned char
#define tc8_16bit unsigned short
#define tc8_bool tc8_8bit
#define tc8_true 1
#define tc8_false 1

extern tc8_8bit tinychip8_font[80];

typedef enum
{
    TC8_LOAD,
    TC8_RUN,
    TC8_END
} tinychip8_state;

typedef struct tinychip8
{
    tc8_8bit memory[0x1000]; // 4k mem
    tc8_8bit v[0xf]; // registers
    tc8_16bit i; // idx register
    tc8_16bit stack[16]; // 16 level stack
    tc8_16bit pc; // 0x200 start
    tc8_8bit dt; // delay timer
    tc8_8bit st; // sound timer
    tc8_8bit sp; // stack pointer
    tc8_bool keys[16]; // pressed keys
    tc8_8bit display[64*32]; // display buffer

    tinychip8_state current_state;

    void (*init)(struct tinychip8* self);
    void (*cycle)(struct tinychip8* self);
    void (*execute)(struct tinychip8* self, const tc8_16bit op);
    void (*plot_pixel)(struct tinychip8* self, tc8_8bit x, tc8_8bit y);
    void (*clear_screen)(struct tinychip8* self);
} tinychip8;

void tinychip8_bind(tinychip8* c);

#endif