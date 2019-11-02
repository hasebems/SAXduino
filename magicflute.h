/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  magicflute.h
 *    description: magicflute
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#ifndef MAGIC_FLUTE_H
#define  MAGIC_FLUTE_H

#include <stdbool.h>
#include <stdint.h>

void initSixTouch( void );
void checkSixTouch( void );

class MagicFlute {
public:
  MagicFlute() : _swState(0), _lastTouch(0), _crntTouch(0), _tapTouch(0),
                 _lastSw(0x24),    //  any touch senser isn't on
                 _crntNote(96), _doremi(12), _nowPlaying(false),
                 _midiExp(0), _startTime(0), _deadBand(0), 
                 _lastSwState(0), _toneNumber(0), _transpose(0),
                 _ledIndicatorCntr(0) {}

  MagicFlute(const MagicFlute& orig);
//  virtual ~MagicFlute(){}

  void    checkSixTouch( void );
  int     midiOutAirPressure( void );
  void    periodic100msec( void );

private:
  void    setNewTouch( uint8_t tch );
  uint8_t getNewNote( void );
  bool    decideDeadBand_byNoteDiff( uint8_t& midiValue, uint32_t crntTime, int diff );
  bool    catchEventOfPeriodic( uint8_t& midiValue, uint32_t crntTime );
  void    analyseSixTouchSens( uint8_t tch );
  void    indicateParticularLed( int num, uint8_t red, uint8_t grn, uint8_t blu );
  void    indicateToneAndTranspose( void );
  void    indicatePitchAndExpression( void );
  void    setNeoPixel( void );

//  void    setCrntNote( uint8_t nt ){ _crntNote = nt;}
//  uint8_t crntNote( void ) const { return _crntNote;}
//  void    setNowPlaying( bool on ){ _nowPlaying = on;}
  bool    nowPlaying( void ) const { return _nowPlaying;}
//  void    setMidiExp( uint8_t mexp ){ _midiExp = mexp;}
//  uint8_t midiExp( void ) const { return _midiExp;}
  uint8_t*  midiExpPtr( void ){ return &_midiExp;}

  static const int _MAX_TOUCH_SW = 6;
  static const int _ALL_CLEAR = _MAX_TOUCH_SW;

  static const unsigned char swTable[];

//  Detect Note
  uint16_t    _swState;     //  raw touch switch state
  uint8_t     _lastTouch;   //  touch state before 10msec
  uint8_t     _crntTouch;   //  current touch state
  uint8_t     _tapTouch;

  uint8_t     _lastSw;    //  Switch (0-0x25) before 10msec
  uint8_t     _crntNote;  //  current note number
  uint8_t     _doremi;

  bool        _nowPlaying;  //  blowing now
  uint8_t     _midiExp;

//  Time Measurement
  uint32_t    _startTime;  //  !=0 means during deadBand
  int         _deadBand;

//  Voice Change / Transpose
  uint8_t     _lastSwState;
  int8_t      _toneNumber;
  int8_t      _transpose;
  uint8_t     _ledIndicatorCntr;  //  0, 1-3, 101-103

};
#endif  /* MAGIC_FLUTE_H */
