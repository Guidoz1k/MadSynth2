/* MIDI values for my Arturia KeyLab Essential MK3 custom ID's

    byte #1 0x90: (A) key on
            0x80: (B) key off
            0xE0: (C) pitch bend
            0xB0: (D) controller

    byte #2 (A): key pressed, from 0x00 (C-2) to 0x7F (G8)
            (B): key pressed, from 0x00 (C-2) to 0x7F (G8)
            (C): pitch bend value LSB
            (D): controller ID
                0x01 for mod wheel,
                0x34 ~ 0x3C for knobs,
                0x66 ~ 0x6E for faders,
                0x10 ~ 0x13 for lower pads,
                0x50 ~ 0x53 for upper pads.

    byte #3 (A): intensity
            (B): 0
            (C): pitch bend value MSB
            (D): controller value
*/

#include "serial.h"

void uarts_setup(void){
    Serial.begin(115200);   // USB uart, will wait for 2 seconds for computer connection
    Serial.setTimeout(0);
}

void serial_string(char *string, bool newline){
    Serial.print(string);
    if(newline == true)
        Serial.print("\n");
}

void serial_number(uint32_t value, uint8_t base, bool newline){
    Serial.print(value, base);
    if(newline == true)
        Serial.print("\n");
}
