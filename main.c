#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "attiny13a_def.h"
#include "attiny13a_9600khz_ws2812b.h"

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

const Color colors[9] PROGMEM = {
    {  10,   0,   0 }, // RED
    {  18,   2,   0 }, // ORANGE
    {  10,  10,   0 }, // YELLOW
    {   0,  10,   0 }, // GREEN
    {   0,  10,  10 }, // CYAN
    {   0,   0,  10 }, // BLUE
    {  10,   0,  10 }, // PURPLE
    {  10,  10,  10 }, // WHITE
    {   0,   0,   0}   // BLACK
};

#define COLOR_RED    0
#define COLOR_ORANGE 1
#define COLOR_YELLOW 2
#define COLOR_GREEN  3
#define COLOR_CYAN   4
#define COLOR_BLUE   5
#define COLOR_PURPLE 6
#define COLOR_WHITE  7
#define COLOR_BLACK  8

ISR(PCINT0_vect) {
    if (PINB_IS_HIGH(PB4)) {
    }
}

volatile uint8_t color = 0;
ISR(WDT_vect) {
    if (++color > 8) color = 0;
}

int main(void) {
    PORTB_SET_INPUT(PB4);
    PORTB_SET_HIGH(PB4);

    DDRB |= 1 << 2;

    WDT_PREPARE_CHANGE();
    WDT_ENABLE_INTERRUPT_1000();

    PCINT0_ENABLE();
    PCINT0_ENABLE_PIN(PCINT4);

    sei();

    while (1) {

        Color current_color;
        pgm_read_block(&colors[color], (void*)&current_color, sizeof(Color));

        for (int i = 0; i < 8; ++i) {
            write<2>(current_color.g);
            write<2>(current_color.r);
            write<2>(current_color.b);
        }
        _delay_ms(100);
    }
}

