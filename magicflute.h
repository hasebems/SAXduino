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
                 _lastNote(0x24),    //  any touch senser isn't on
                 _startTime(0), _deadBand(0), _touchCurrentStatus(0),
                 _crntNote(96), _doremi(12), _nowPlaying(false),
                 _midiExp(0) {}

  MagicFlute(const MagicFlute& orig);
  virtual ~MagicFlute(){}

  void    checkSixTouch( void );
  int     midiOutAirPressure( void );
  void    periodic100msec( void );
  
  void    setNewTouch( uint8_t tch );
  uint8_t getNewNote( void );
  bool    catchEventOfPeriodic( uint8_t& midiValue, uint32_t crntTime );

  void    setCrntNote( uint8_t nt ){ _crntNote = nt;}
  uint8_t crntNote( void ) const { return _crntNote;}
  void    setNowPlaying( bool on ){ _nowPlaying = on;}
  bool    nowPlaying( void ) const { return _nowPlaying;}
  void    setMidiExp( uint8_t mexp ){ _midiExp = mexp;}
  uint8_t midiExp( void ) const { return _midiExp;}
  uint8_t*  midiExpPtr( void ){ return &_midiExp;}

private:
  void    analyseSixTouchSens( uint8_t tch );
  void    setNeoPixelExp( uint8_t note, uint8_t exprs );
  
  static const unsigned char swTable[64];

  uint16_t     _swState;

  uint8_t     _lastTouch;
  uint8_t     _crntTouch;
  uint8_t     _tapTouch;
  uint8_t     _lastNote;

  uint8_t     _touchCurrentStatus;
  uint8_t     _crntNote;
  uint8_t     _doremi;
  bool        _nowPlaying;

  uint8_t     _midiExp;

//  Time Measurement
  uint32_t    _startTime;  //  !=0 means during deadBand
  int         _deadBand;
};
#endif  /* MAGIC_FLUTE_H */
