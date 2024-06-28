#ifndef __UMIDI_H
#define __UMIDI_H

#include <Arduino.h>

typedef enum /* midi command identifier */ {
    MIDI_ID_INVALID     = 0,    // no command identified
    MIDI_ID_KEYON       = 1,    // key pressed
    MIDI_ID_KEYOFF      = 2,    // key released
    MIDI_ID_PITCHWHEEL  = 3,    // pitch wheel changed
    MIDI_ID_MODWHEEL    = 4,    // mod wheel changed
    MIDI_ID_KNOB        = 5,    // one of the 9 knob controllers changed
    MIDI_ID_FADER       = 6,    // one of the 9 fader controllers changed
    MIDI_ID_UPPERPAD    = 7,    // upper pad change
    MIDI_ID_LOWERPAD    = 8,    // lower pad change
    MIDI_ID_NOMESSAGE   = 255,
} midi_id_t;

typedef struct /* midi message struct */ {
    midi_id_t midi_command = MIDI_ID_INVALID;
    uint8_t firstbyte = 0;
    uint8_t secondbyte = 0;
} midi_input_t;

void uart_midi_setup(void);

midi_input_t uart_midi(void);

#endif /* __UMIDI_H */