/*
 * Code of "write" function is extracted from Alex Gyver repository
 * https://github.com/GyverLibs/microLED/blob/main/src/tinyLED.h
 */
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
