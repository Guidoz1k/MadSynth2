#include "pcm.h"

// audio api library
#include <Audio.h>
#include <utility/imxrt_hw.h> // for the I²S frequency change

#include "umidi.h"
#include "synth_dds.h"

// =============================================== variables ===============================================

// interrupt variables
IntervalTimer timer;
synth_mode_t int_mode = SYNTH_DDS;
uint32_t interrupt_counter = 0; // incremented every subdivision of the 10ms total interrupt time. Right now, 1ms
float32_t volume = 0.5;

// audio api variables
AudioPlayQueue           leftqueue;
AudioPlayQueue           rightqueue;
AudioOutputI2S           i2s1;
AudioConnection          patchCord2(rightqueue, 0, i2s1, 0);
AudioConnection          patchCord1(leftqueue, 0, i2s1, 1);

// MIDI controller
uint16_t midi_pitch_wheel = 0;
uint8_t midi_mod_wheel = 0;
uint8_t midi_faders[9] = {0};
uint8_t midi_knobs[9] = {0};

/* =============================================== INTERRUPT ===============================================
    Timer ISR should add 441 positions on each queue every 10ms, for 44.1kHz
    By filling 440 positions in subdivisions and adding 1 extra in the last one, it is
    possible to use smaller interrupt periods. Right now it is doing 10 subdivisions,
    loading 44 samples to the I²S bus in the first 9 and 45 samples in the last one.
    As it is 10 subdivisions on a 10ms period, subdivisions will occur every 1ms.
*/

void parse_midi_DDS(void){
    midi_input_t midi_in;

    midi_in = uart_midi();
    
    switch(midi_in.midi_command){
    case MIDI_ID_KEYON:
        volume = 1.0;
        set_DDSoscillator(midi_in.firstbyte);
        break;
    case MIDI_ID_KEYOFF:
        clear_DDSoscillator(midi_in.firstbyte);
        break;
    case MIDI_ID_PITCHWHEEL:
        midi_pitch_wheel = midi_in.firstbyte + 256 * midi_in.secondbyte;
        break;
    case MIDI_ID_MODWHEEL:
        midi_mod_wheel = midi_in.secondbyte;
        break;
    case MIDI_ID_KNOB:
        midi_knobs[midi_in.firstbyte] = midi_in.secondbyte;
        break;
    case MIDI_ID_FADER:
        midi_faders[midi_in.firstbyte] = midi_in.secondbyte;
        break;
    case MIDI_ID_UPPERPAD:
        /*
        switch(midi_in.firstbyte){
        case 0:
            set_DDSwaveform(WAVE_NONE, WAVE_SINE);
            break;
        case 1:
            set_DDSwaveform(WAVE_NONE, WAVE_TRIANG);
            break;
        case 2:
            set_DDSwaveform(WAVE_NONE, WAVE_SQUARE);
            break;
        case 3:
            set_DDSwaveform(WAVE_NONE, WAVE_SAW);
            break;
        }
        */
        break;
    case MIDI_ID_LOWERPAD:
        /*
        switch(midi_in.firstbyte){
        case 0:
            set_DDSwaveform(WAVE_SINE, WAVE_NONE);
            break;
        case 1:
            set_DDSwaveform(WAVE_TRIANG, WAVE_NONE);
            break;
        case 2:
            set_DDSwaveform(WAVE_SQUARE, WAVE_NONE);
            break;
        case 3:
            set_DDSwaveform(WAVE_SAW, WAVE_NONE);
            break;
        }
        */
        break;
    default:
        break;
    }

}

void timer_interrupt(){
    int16_t buffer_r[192] = {0};
    int16_t buffer_l[192] = {0};

    // external monitoring for interrupt processing time
    digitalWriteFast(INTPIN, HIGH);

    interrupt_counter++;    // a 1ms counter for the system

    // different synthesis modes are switched here
    switch(int_mode){
    case SYNTH_DDS:
        DDS_mode(192, buffer_r, buffer_l);
        parse_midi_DDS();           // reads MIDI inputs
        break;
    case SYNTH_PHYS_STRING:
        // PHYS_STRING_mode(192, buffer_r, buffer_l);
        // parse_midi_SYNTH_PHYS_STRING();           // reads MIDI inputs
        break;
    default:
        break;
    }

    // load buffers in the I²S peripheral
    leftqueue.play(buffer_l, 192);
    rightqueue.play(buffer_r, 192);

    // external monitoring for interrupt processing time
    digitalWriteFast(INTPIN, LOW);
}

// =============================================== functions ===============================================

uint32_t return_int_time(void){
    return interrupt_counter;
}

void set_volume(float32_t vol){
    volume = vol;
}

// https://forum.pjrc.com/index.php?threads/change-sample-rate-for-teensy-4-vs-teensy-3.57283/
void setI2SFreq(int freq) {
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (freq * 256 * n1);
  double C = ((double)freq * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f
}

void pcm_setup(void){
    pinMode(INTPIN, OUTPUT);
	AudioMemory(64);
    timer.begin(timer_interrupt, 1000); // 1000µs = 1ms tick rate for the interrupt
    setI2SFreq(192000); // sample rate = 192kHz

    DDS_setup();
}

