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

static inline void pgm_read_block(const void *s, void *dest, uint8_t len) {
    uint8_t *dp = (uint8_t *)dest;
    for (uint8_t i=0; i<len; i++) {
        dp[i] = pgm_read_byte(i + (const uint8_t *)s);
    }
}

