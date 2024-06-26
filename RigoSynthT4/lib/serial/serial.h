#ifndef __SERIAL_H
#define __SERIAL_H

#include <Arduino.h>

typedef enum /* midi command identifier */ {
    INVALID     = 0,    // no command identified
    KEYON       = 1,    // key pressed
    KEYOFF      = 2,    // key released
    PITCHWHEEL  = 3,    // pitch wheel changed
    MODWHEEL    = 4,    // mod wheel changed
    KNOB        = 5,    // one of the 9 knob controllers changed
    FADER       = 6,    // one of the 9 fader controllers changed
    UPPERPAD    = 7,    // upper pad change
    LOWERPAD    = 8,    // lower pad change
    NOMESSAGE   = 255,
} midi_id_t;

typedef struct /* midi message struct */ {
    midi_id_t midi_command = INVALID;
    uint8_t firstbyte = 0;
    uint8_t secondbyte = 0;
} midi_input_t;

void uarts_setup(void);

void serial_string(char *string, bool newline);

void serial_number(uint32_t value, uint8_t base, bool newline);

#endif /* __SERIAL_H */