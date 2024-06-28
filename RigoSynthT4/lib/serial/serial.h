#ifndef __SERIAL_H
#define __SERIAL_H

#include <Arduino.h>

void uarts_setup(void);

void serial_string(char *string, bool newline);

void serial_number(uint32_t value, uint8_t base, bool newline);

#endif /* __SERIAL_H */