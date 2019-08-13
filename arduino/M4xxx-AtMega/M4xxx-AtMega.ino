/*
 * Music 4000 Midi Keyboard Adapter
 * For the BBC Micro computer
 * ATMega328P version
 * 
 *  Copyright Jason Lane 2019
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/ or send a letter to 
 *  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
 * 
 */
#include <SPI.h>
#include <elapsedMillis.h>
#include <EEPROM.h>
#include "pintimer.h"

//#define DEBUG 1

// Pins
#define LEDpin 4
#define SWpin 5
#define FS1pin 3
#define FS2pin 2
#define SRlatch 10
#define SRdata 11
#define SRclk 13
#define MIDIin 0
#define MIDI_MIN_NOTE 36
#define MIDI_MAX_NOTE 84
#define MIDI_NOTEON 0x90
#define MIDI_NOTEOFF 0x80
#define MIDI_CTRLCHG 0xB0

uint8_t noteOnCmd  = 0x90;
uint8_t noteOffCmd = 0x80;
uint8_t controlChangeCmd = 0xB0;
uint8_t allNotesOffCmd = 0x7B;

#define CHANNEL_ADDR 1
volatile uint8_t midiChannel = 0;
boolean setMIDIchan = false;

PinTimer led(LEDpin);
uint8_t KB[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
bool kbChanged;

void updateKB() {
  // Send current KB status to shift registers
  SPI.beginTransaction( SPISettings(10000000, MSBFIRST, SPI_MODE0) );
  digitalWrite( SRlatch, LOW );
  for (uint8_t i=0; i<8; i++)  SPI.transfer( KB[i] );
  digitalWrite( SRlatch, HIGH );
  SPI.endTransaction();
}

void updateLED() {
  uint8_t i = 0;
  bool led = false;
  for (i=1; i<8; i++) {
    if (KB[i] != 0xFF) led = true;
  }
  digitalWrite( LEDpin, led );
}

void keyOff( uint8_t k ) {
  uint8_t kblock = k / 8;
  uint8_t kbit = k % 8;
  bitSet( KB[7-kblock], kbit );
  kbChanged = true;
}

void keyOn( uint8_t k ) {
  uint8_t kblock = k / 8;
  uint8_t kbit = k % 8;
  bitClear( KB[7-kblock], kbit );
  kbChanged = true;
}
 
void clearNotes() {
  for (uint8_t i=0; i<8; i++) KB[i] = 0xFF;
}

void parseMIDI() {
  // Call when serial data is available
  static uint8_t state=0;
  static uint8_t cmd=0;
  static uint8_t note=0;
  static uint8_t vel=0;
  uint8_t m = Serial.read();
  if (m>=0x80 && m<=0xEF) {
    // Voice Category Status
    cmd = m;
    state = 0; 
  } else if (m>=0xF0 && m<=0xF7) {
    // System Common Status
    cmd = 0;
    state = 0;
  }
  if (cmd>0) {
    switch (state) {
      case 0:
        // cmd read
        if (cmd==MIDI_NOTEON) {
          state = 1;
        } else if (cmd==MIDI_NOTEOFF) {
          state = 3;
        } else if (cmd==MIDI_CTRLCHG) {
          state = 5;
        } else {
          // Not recognised command so reset
          cmd = 0;
        }
        break;
      case 1:
        // read note
        note = m;
        state=2;
        break;
      case 2:
        // read vel
        vel = m;
        state = 1;
        if (note>=MIDI_MIN_NOTE && note <=MIDI_MAX_NOTE) {
          // Only use range available for keyboard
          if (vel>0) {
            // Note on
            keyOn(note-MIDI_MIN_NOTE);
          } else {
            // Note off
            keyOff(note-MIDI_MIN_NOTE);
          }
        }
      break;
      case 3:
        // Note Off read note
        note = m;
        state = 4;
      break;
      case 4:
        // Note Off read vel
        vel = m;
        keyOff(note-MIDI_MIN_NOTE);        
        state = 3;
      break;
      case 5:
        // Control change
        if (m>=0x7B && m<=0x7F) {
          // Clear all notes
          clearNotes();
        }
        state = 0;
      break;
    }
  }
}

void updateFootswitch() {
  // Check footswitch status every 40mS
  static elapsedMillis timer = 0;
  if (timer<40) return;
  timer = 0;
  static bool FS1status = digitalRead(FS1pin);
  static bool FS2status = digitalRead(FS2pin);
  bool FS1 = digitalRead(FS1pin);
  bool FS2 = digitalRead(FS2pin);
  if (FS1 != FS1status) {
    FS1status = FS1;
    if (!FS1) keyOn(63); else keyOff(63);
  }
  if (FS2 != FS2status) {
    FS2status = FS2;
    if (!FS2) keyOn(62); else keyOff(62);
  }
}

uint8_t readMIDIchannel() {
  uint8_t chan = EEPROM.read(CHANNEL_ADDR);
  if (chan>=16) {
    chan=0;
  }
  noteOnCmd  = 0x90|chan;
  noteOffCmd = 0x80|chan;
  controlChangeCmd = 0xB0|midiChannel;
  return chan;
}


void writeMIDIchannel(uint8_t chan) {
  // Store the MIDI channel
  EEPROM.write(CHANNEL_ADDR,chan);
}

elapsedMillis btnTimer = 0;
void handleButton() {
  // State machine to handle setting the MIDI channel
  // Unpressed, button is HIGH.
  // Hold button down for 3 secs to enter channel set mode
  static uint8_t state = 0;
  switch (state) {
    case 0:
      // Wait for button to be pressed
      if (digitalRead(SWpin)==LOW) {
        btnTimer = 0;
        state = 1;
      }
      break;
    case 1:
      // Wait for release or button held > 4 seconds
      if (digitalRead(SWpin)==HIGH) {
        state = 0;
      } else if (btnTimer>4000) {
        // start MIDI set mode
        state = 2;
        setMIDIchan = true;
        btnTimer = 0;
        led.oscillate(250);
      }
      break;
    case 2: 
      // wait for timeout 
      if (btnTimer>10000 || !setMIDIchan) {
        setMIDIchan = false;
        state = 0;
        led.off();
      }
      break;
  } 
}

void setup() {
  led.on();
  digitalWrite( LEDpin, HIGH );
  Serial.begin(31250); // Start MIDI on hardware serial
  SPI.begin();
  pinMode( SRlatch, OUTPUT );
  digitalWrite( SRlatch,HIGH );
  pinMode( SWpin, INPUT_PULLUP );
  pinMode( FS1pin, INPUT_PULLUP );
  pinMode( FS2pin, INPUT_PULLUP );
  updateKB();
  if (digitalRead(SWpin)) {
    midiChannel = readMIDIchannel();
  } else {
    // reset channel to 0
    midiChannel = 0;
    writeMIDIchannel( midiChannel );
  }
  delay(500);
  led.off();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    parseMIDI();
  }
  updateFootswitch();
  if (kbChanged) {
    updateKB();
    updateLED();
    kbChanged = false;
  }
  handleButton();

}
