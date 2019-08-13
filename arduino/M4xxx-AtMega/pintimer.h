/* 
 * Simply handle pulsing and oscillating a digital pin
 * e.g. for trigger pulses or flashing LEDs. 
 * Uses the Teensy elapsedMillis and elapsedMicros types
 *   
 * The digital pin is set in the constructor
 * Call update() in the main program loop. 
 * The update runs at least every 500uS
 * Timings for osillating and pulsing are in mS
 * Pulse will revert to previous state (oscillating, or off) when finished
 * Oscillate will start switching the pin on and off with a period of 2x time
 * until off or pulse are called.
 * Simple 3 state machine - 0=idle or off; 1=oscillating; 2=pulse.
 *  
 *  Copyright Jason Lane 2017
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/ or send a letter to 
 *  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
 */

#ifndef Pintimer_h
#define Pintimer_h

#include <Arduino.h>
#include <elapsedMillis.h>


#define PT_UPDATEMICROS 500


class PinTimer
{
    public:
        PinTimer(uint8_t pin);
        void update();          // update the state machine
        void on();                   // switch on pin
        void off();                  // switch off pin
        void pulse( uint16_t time ); // switches pin on for time. Reverts back to previous state
        void oscillate( uint16_t time ); // oscillates pin for time
        void oscillate( uint16_t time, uint8_t flashes );
       
    private:  
  uint8_t _pin;           // output pin
        boolean _status;        // state of pin - HIGH or LOW
        uint8_t _state;     // state machine
        uint8_t _oldState;  // state to revert to after pulse
        uint16_t _oscTime;      // time to oscillate
        uint16_t _flashTime;
        uint16_t _pulseTime;    // time for on-pulse
        elapsedMicros _updTime; // update timer
        elapsedMillis _timer;   // pulse timer
        uint8_t _flashCount;
};

#endif
