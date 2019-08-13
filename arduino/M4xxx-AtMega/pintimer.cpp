/* 
 *  Simply handle pulsing and oscillating a digital pin
 *    
 *  Copyright Jason Lane 2017
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/ or send a letter to 
 *  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
 * 
 */

#include <Arduino.h>

#include "pintimer.h"


PinTimer::PinTimer(uint8_t pin) {
  // Constructor
  _pin = pin;
  pinMode( _pin, OUTPUT );
  _state = 0;
  _status = LOW;
  digitalWrite( _pin, LOW );
}
  
void PinTimer::update() {
  // update the state machine every so often
  if (_updTime>PT_UPDATEMICROS) {
    _updTime = 0;
    switch( _state ) {
      case 1:
        // oscillate
        if (_timer >= _oscTime) {
          _timer = 0;
          _status = !_status;
          digitalWrite(_pin,_status);
        }
        break;
      case 2:
        // pulse
        if (_timer >= _pulseTime) {
          _status = LOW;
          digitalWrite(_pin,LOW);
          _state = _oldState;
        }
        break;
      case 3:
        // count flashes
        if (_timer >= _flashTime) {
          _timer = 0;
          _status = !_status;
          digitalWrite(_pin,_status);
          if (!_status) {
            _flashCount--;
            if (_flashCount==0) {
              _state = _oldState;
            }
          }
        }
        break;

    }
  }
}

void PinTimer::on() {
  // switch on pin
  _status = HIGH;
  digitalWrite( _pin, HIGH );
  _state = 0;
}

void PinTimer::off() {
  // switch off pin
  _status = LOW;
  digitalWrite( _pin, LOW );
  _state = 0;
}

void PinTimer::pulse( uint16_t time ) {
  // switches pin for on time
  if (_state !=2) {
    _oldState = _state;
    _state = 2;
    _status = HIGH;
    digitalWrite( _pin, HIGH );
  }
  _pulseTime = time;
  _timer = 0;
}

void PinTimer::oscillate( uint16_t time ) {
  // starts oscillating pin for time
  _status = HIGH;
  digitalWrite( _pin, HIGH );
  _state = 1;
  _oscTime = time;
  _timer = 0;
}

void PinTimer::oscillate( uint16_t time, uint8_t flashes ) {
  // starts oscillating pin for time
  if (_state !=3) {
    _oldState = _state;
    _status = HIGH;
    digitalWrite( _pin, HIGH );
    _state = 3;
    _flashCount = flashes;
    _flashTime = time;
    _timer = 0;
  }
}
