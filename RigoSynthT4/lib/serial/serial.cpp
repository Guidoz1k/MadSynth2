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

#define MIDISERIALPORT Serial

#define TIMEOUT 2   // uart timeout in milliseconds
#define RETRIES 3   // retries of midi read function until it flushes the buffer

void uarts_setup(void){
    Serial.begin(115200);   // USB uart, will wait for 2 seconds for computer connection
    Serial.setTimeout(TIMEOUT);
    Serial7.begin(31250);   // MIDI uart
    Serial7.setTimeout(TIMEOUT);
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

midi_input_t uart_midi(void){
    char buffer[3] = {0};
    midi_input_t midi_message;
    static uint8_t retries = 0;

    if(MIDISERIALPORT.available() == true){ // there is at least one char available at uart input buffer
        if(MIDISERIALPORT.readBytes(buffer, 3) == 3){   // there are 3 chars available at uart input buffer
            switch(buffer[0]){
            case 0x90:  // key on
                midi_message.midi_command = KEYON;
                midi_message.firstbyte = buffer[1];
                midi_message.secondbyte = buffer[2];
                break;
            case 0x80:  // key off
                midi_message.midi_command = KEYOFF;
                midi_message.firstbyte = buffer[1];
                midi_message.secondbyte = buffer[2];
                break;
            case 0xE0:  // pitch bend
                midi_message.midi_command = PITCHWHEEL;
                midi_message.firstbyte = buffer[1];
                midi_message.secondbyte = buffer[2];
                break;
            case 0xB0:  // controller
                if(buffer[1] == 0x01){  // mod wheel
                    midi_message.midi_command = MODWHEEL;
                    midi_message.firstbyte = buffer[1] - 1;
                    midi_message.secondbyte = buffer[2];
                }
                else{
                    if( (buffer[1] >= 0x34) && (buffer[1] <= 0x3C) ){ // knobs
                        midi_message.midi_command = KNOB;
                        midi_message.firstbyte = buffer[1] - 0x34;
                        midi_message.secondbyte = buffer[2];
                    }
                    else{
                        if( (buffer[1] >= 0x66) && (buffer[1] <= 0x6E) ){ // faders
                            midi_message.midi_command = FADER;
                            midi_message.firstbyte = buffer[1] - 0x66;
                            midi_message.secondbyte = buffer[2];
                        }
                        else{
                            if( (buffer[1] >= 0x10) && (buffer[1] <= 0x13) ){ // lower pads
                                midi_message.midi_command = LOWERPAD;
                                midi_message.firstbyte = buffer[1] - 0x10;
                                midi_message.secondbyte = buffer[2];
                            }
                            else{
                                if( (buffer[1] >= 0x50) && (buffer[1] <= 0x53) ){ // upper pads
                                    midi_message.midi_command = UPPERPAD;
                                    midi_message.firstbyte = buffer[1] - 0x50;
                                    midi_message.secondbyte = buffer[2];
                                }
                                else{
                                    midi_message.midi_command = INVALID;    // controller not identified
                                }
                            }
                        }
                    }
                }
                break;
            default: // first byte is not identified
                MIDISERIALPORT.flush();
                break;
            }
        }
        else{   // there isn't 3 chars available at uart input buffer
            if(++retries == RETRIES){
                MIDISERIALPORT.flush();
                retries = 0;
            } 
        }
    }

    return midi_message;
}
