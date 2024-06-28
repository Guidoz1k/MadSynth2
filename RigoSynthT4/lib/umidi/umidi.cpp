#include "umidi.h"

void uart_midi_setup(void){
    Serial7.begin(31250);   // MIDI uart
    Serial7.setTimeout(0);
}

midi_input_t uart_midi(void){
    char buffer[3] = {0};
    midi_input_t midi_message;

    if(Serial7.available() >= 3){  // there are 3 chars available at uart input buffer
        Serial7.readBytes(buffer, 3);
        switch(buffer[0]){
        case 0x90:  // key on
            midi_message.midi_command = MIDI_ID_KEYON;
            midi_message.firstbyte = buffer[1];
            midi_message.secondbyte = buffer[2];
            break;
        case 0x80:  // key off
            midi_message.midi_command = MIDI_ID_KEYOFF;
            midi_message.firstbyte = buffer[1];
            midi_message.secondbyte = buffer[2];
            break;
        case 0xE0:  // pitch bend
            midi_message.midi_command = MIDI_ID_PITCHWHEEL;
            midi_message.firstbyte = buffer[1];
            midi_message.secondbyte = buffer[2];
            break;
        case 0xB0:  // controller
            if(buffer[1] == 0x01){  // mod wheel
                midi_message.midi_command = MIDI_ID_MODWHEEL;
                midi_message.firstbyte = buffer[1] - 1;
                midi_message.secondbyte = buffer[2];
            }
            else{
                if( (buffer[1] >= 0x34) && (buffer[1] <= 0x3C) ){ // knobs
                    midi_message.midi_command = MIDI_ID_KNOB;
                    midi_message.firstbyte = buffer[1] - 0x34;
                    midi_message.secondbyte = buffer[2];
                }
                else{
                    if( (buffer[1] >= 0x66) && (buffer[1] <= 0x6E) ){ // faders
                        midi_message.midi_command = MIDI_ID_FADER;
                        midi_message.firstbyte = buffer[1] - 0x66;
                        midi_message.secondbyte = buffer[2];
                    }
                    else{
                        if( (buffer[1] >= 0x10) && (buffer[1] <= 0x13) ){ // lower pads
                            midi_message.midi_command = MIDI_ID_LOWERPAD;
                            midi_message.firstbyte = buffer[1] - 0x10;
                            midi_message.secondbyte = buffer[2];
                        }
                        else{
                            if( (buffer[1] >= 0x50) && (buffer[1] <= 0x53) ){ // upper pads
                                midi_message.midi_command = MIDI_ID_UPPERPAD;
                                midi_message.firstbyte = buffer[1] - 0x50;
                                midi_message.secondbyte = buffer[2];
                            }
                            else{
                                midi_message.midi_command = MIDI_ID_INVALID;    // controller not identified
                            }
                        }
                    }
                }
            }
            break;
        default: // first byte is not identified
            //Serial7.flush();
            break;
        }
    }

    return midi_message;
}
