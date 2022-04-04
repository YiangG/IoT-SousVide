

#include <avr/io.h>

uint8_t therm_reset();

void therm_write_bit(uint8_t bit);

uint8_t therm_read_bit(void);

uint8_t therm_read_byte(void);

void therm_write_byte(uint8_t byte);

void therm_read_temperature(char *buffer);
