#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

template<uint8_t PIN>
void write(uint8_t data) {
    uint8_t oledSreg = SREG;
    cli();
    asm volatile
    (
        "LDI r19,8               \n\t"   // Счетчик 8ми циклов
        "_LOOP_START_%=:      \n\t"   // Начало цикла
        "SBI %[PORT], %[PIN]  \n\t"   // HIGH на выход
        "SBRS %[DATA], 7      \n\t"   // Если бит '7' установлен, пропуск след. инструкции
        "CBI %[PORT], %[PIN]  \n\t"   // LOW на выход
        "LDI r20, 1            \n\t"
        "_DELAY_LOOP_%=:    \n\t"     // Цикл задержки
        "DEC r20               \n\t"     // 1CK декремент
        "BRNE _DELAY_LOOP_%=\n\t"     // 2CK переход
        "CBI %[PORT], %[PIN]    \n\t"   // LOW на выход
        "LSL %[DATA]            \n\t"   // Сдвигаем данные влево
        "DEC r19                 \n\t"   // Декремент счетчика циклов
        "BRNE _LOOP_START_%=    \n\t"   // Переход в начало цикла
        :
        :[DATA]"r"(data),
        [PORT]"I"(_SFR_IO_ADDR(PORTB)),
        [PIN]"I"(PIN)
        :"r19","r20"
    );
    SREG = oledSreg;
}

#define PCINT0_CLEANUP() GIFR |= (1 << PCIF)
#define PCINT0_ENABLE() GIMSK |= (1 << PCIE)
#define PCINT0_DISABLE() GIMSK &= ~(1 << PCIE)
#define PCINT0_ENABLE_PIN(pin) PCMSK |= (1 << pin)

#define PORTB_SET_OUTPUT(pin) DDRB |= (1 << pin)
#define PORTB_SET_INPUT(pin) DDRB &= ~(1 << pin)
#define PORTB_TOGGLE(pin) PORTB ^= (1 << pin)
#define PORTB_SET_HIGH(pin) PORTB |= (1 << pin)

#define PINB_IS_HIGH(pin) (PINB & (1 << pin))

#define WDT_DISABLE() WDTCR = 0x0
#define WDT_PREPARE_CHANGE() WDTCR = (1 << WDCE)
#define WDT_ENABLE_INTERRUPT_16() WDTCR =  (1 << WDTIE)
#define WDT_ENABLE_INTERRUPT_32() WDTCR =  (1 << WDTIE) | (1 << WDP0)
#define WDT_ENABLE_INTERRUPT_64() WDTCR =  (1 << WDTIE) | (1 << WDP1)
#define WDT_ENABLE_INTERRUPT_125() WDTCR = (1 << WDTIE) | (1 << WDP0) | (1 << WDP1)
#define WDT_ENABLE_INTERRUPT_250() WDTCR = (1 << WDTIE) | (1 << WDP2)
#define WDT_ENABLE_INTERRUPT_500() WDTCR = (1 << WDTIE) | (1 << WDP0) | (1 << WDP2)
#define WDT_ENABLE_INTERRUPT_1000() WDTCR = (1 << WDTIE) | (1 << WDP2) | (1 << WDP1)
#define WDT_ENABLE_INTERRUPT_2000() WDTCR = (1 << WDTIE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0)
#define WDT_ENABLE_INTERRUPT_4000() WDTCR = (1 << WDTIE) | (1 << WDP3)
#define WDT_ENABLE_INTERRUPT_8000() WDTCR = (1 << WDTIE) | (1 << WDP3) | (1 << WDP0)

#define TIMER_0_NMODE_TOGGLE_OC0A() TCCR0A |= (1 << COM0A0)

#define TIMER_0_CTC_MODE_ENABLE() TCCR0A |= (1 << WGM01)
#define TIMER_0_FAST_PWM_TOP_MODE_ENABLE() TCCR0A |= (1 << WGM01) | (1 << WGM00)

#define TIMER_0_FAST_PWM_MODE_CLEAR_SET() TCCR0A |= (1 << COM0A1)
#define TIMER_0_FAST_PWM_MODE_SET_CLEAR() TCCR0A |= (1 << COM0A1)

#define TIMER_0_TOP_A_VALUE(value) OCR0A = value

#define TIMER_0_NO_PRESCALE() TCCR0B = (1 << CS00)
#define TIMER_0_8_PRESCALE()  TCCR0B = (1 << CS01)
#define TIMER_0_64_PRESCALE()  TCCR0B = (1 << CS01) | (1 << CS00)
#define TIMER_0_256_PRESCALE()  TCCR0B = (1 << CS02)
#define TIMER_0_1024_PRESCALE()  TCCR0B = (1 << CS02) | (1 << CS00)
#define TIMER_0_EXTERNAL_FALLING()  TCCR0B = (1 << CS02) | (1 << CS01)
#define TIMER_0_EXTERNAL_RISING()  TCCR0B = (1 << CS02) | (1 << CS01) | (1 << CS00)

#define UNDEFINED 0xFF

#define MODE_UNDEFINED 0
#define MODE_NORMAL 1
#define MODE_CTC 2
#define MODE_FAST_PWM 3
#define MODE MODE_FAST_PWM

#define UNDEFINED 0xFF

static inline void pgm_read_block(const void *s, void *dest, uint8_t len) {
    uint8_t *dp = (uint8_t *)dest;
    for (uint8_t i=0; i<len; i++) {
        dp[i] = pgm_read_byte(i + (const uint8_t *)s);
    }
}

#define CH_0 0xFF
#define CH_R 0
#define CH_G 1
#define CH_B 2

#define WAVE_LEN 24
#define MODE_COUNT 9

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

