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
    if (!(ADCSRA & (1 << ADSC) && !PINB_IS_HIGH(PB4))) {
        _delay_ms(1);
        ADCSRA |= (1 << ADSC);
    }
}

ISR(WDT_vect) {
    // empty
}

static void menu_key(uint8_t v);
ISR(ADC_vect) {
    if (!(ADCSRA & (1 << ADSC))) {
      menu_key(ADCH);
    }
}

#define KEY_NONE  0
#define KEY_1     5
#define KEY_2     15
#define KEY_3     30
#define KEY_4     45
#define KEY_5     90

volatile uint8_t menu[6] = {0, 1, 2, 3, 4, 5};
volatile uint8_t menu_counter = 0;

static void menu_key(uint8_t v) {
    if (v < KEY_1) {
        if (menu[menu_counter] < 8) ++menu[menu_counter];
    } else if (v < KEY_2) {
        if (menu_counter < 5) ++menu_counter;
    } else if (v < KEY_3) {
        if (menu_counter > 0) --menu_counter;
    } else if (v < KEY_4) {
        if (menu[menu_counter] > 0) --menu[menu_counter];
    } else if (v < KEY_5) {

    }
}

int main(void) {
    PORTB_SET_INPUT(PB4);
    PORTB_SET_HIGH(PB4);

    DDRB |= 1 << 2;

    WDT_PREPARE_CHANGE();
    WDT_ENABLE_INTERRUPT_1000();

    PCINT0_ENABLE();
    PCINT0_ENABLE_PIN(PCINT4);

    ADMUX =
        (1 << ADLAR) |
        (1 << MUX1);

    ADCSRA =
        (1 << ADEN)  |
        (1 << ADPS2) |
        (1 << ADPS1) |
        (1 << ADIE);

    ADCSRA |= (1 << ADSC);

    sei();

    while (1) {
        Color current_color;
        pgm_read_block(&colors[menu_counter], (void*)&current_color, sizeof(Color));

        for (int i = 0; i < 8; ++i) {
            write<2>(menu[menu_counter] <= i ? current_color.g : 0);
            write<2>(menu[menu_counter] <= i ? current_color.r : 0);
            write<2>(menu[menu_counter] <= i ? current_color.b : 0);
        }
        _delay_ms(100);
    }
}

