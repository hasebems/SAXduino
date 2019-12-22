/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  TouchMIDI_AVR_if.h
 *    description: TouchMidi Interface Functions 
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#ifndef TOUCH_MIDI_AVR_IF_H
#define TOUCH_MIDI_AVR_IF_H
 
#include <Arduino.h>

int analogDataRead( void );
void displayError( void );
void setAda88_Number( int );
void setMidiBuffer( uint8_t dt0, uint8_t dt1, uint8_t dt2 );
void setMute( bool mute );

//  for NeoPixel
uint8_t colorTbl( uint8_t index, uint8_t rgb );
void setLed( int ledNum, uint8_t red, uint8_t green, uint8_t blue );
void lightLed( void );


class GlobalTimer {

public:
  GlobalTimer( void ) : _globalTime(0), _timer100msec(0), _timer100msec_sabun(0),
                        _timer1sec(0), _timer1sec_sabun(0), _timer10msec_event(false),
                        _timer100msec_event(false), _timer1sec_event(false) {}

  void      incGlobalTime( void ){ _globalTime++;}
  uint32_t  readGlobalTimeAndClear( void )
  {
    noInterrupts();
    uint32_t tm = _globalTime;
    _globalTime = 0;
    interrupts();
    return tm;
  }
  void      setTimer100ms( uint16_t tm ){ _timer100msec = tm;}
  uint32_t  timer10ms( void ) const { return _timer10msec;}
  uint32_t  timer100ms( void ) const { return _timer100msec;}
  uint32_t  timer1s( void ) const { return _timer1sec;}
  uint8_t   globalTime( void ) const { return _globalTime;}

  void      clearAllTimerEvent( void ){ _timer10msec_event = _timer100msec_event = _timer1sec_event = false;}
  bool      timer10msecEvent( void ) const { return _timer10msec_event;}
  bool      timer100msecEvent( void ) const { return _timer100msec_event;}
  bool      timer1secEvent( void ) const { return _timer1sec_event;}
  
  void      updateTimer( uint32_t diff )
  {
    if ( diff > 0 ){
      _timer10msec += diff;
      _timer10msec_event = true;
    }

    _timer100msec_sabun += (uint16_t)diff;
    while ( _timer100msec_sabun > 10 ){
      _timer100msec++;
      _timer100msec_event = true;
      _timer100msec_sabun -= 10;
    }
    _timer1sec_sabun += (uint16_t)diff;
    while ( _timer1sec_sabun > 100 ){
      _timer1sec++;
      _timer1sec_event = true;
      _timer1sec_sabun -= 100;
    }
  }

  bool      _timer10msec_event;
  bool      _timer100msec_event;
  bool      _timer1sec_event;

private:

  volatile uint8_t  _globalTime;
  uint32_t  _timer10msec;
  uint32_t  _timer100msec;
  uint32_t  _timer100msec_sabun;
  uint32_t  _timer1sec;
  uint32_t  _timer1sec_sabun;
};
#endif
