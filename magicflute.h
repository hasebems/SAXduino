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
                 _vceChangeProcess(0) {}

  MagicFlute(const MagicFlute& orig);
  virtual ~MagicFlute(){}

  void    checkSixTouch( void );
  int     midiOutAirPressure( void );
  void    periodic100msec( void );

private:
  void    setNewTouch( uint8_t tch );
  uint8_t getNewNote( void );
  bool    catchEventOfPeriodic( uint8_t& midiValue, uint32_t crntTime );
  void    analyseSixTouchSens( uint8_t tch );
  void    setVoiceChangeProcess( uint8_t newNote, uint8_t oldNote );
  void    setNeoPixelExp( uint8_t note, uint8_t exprs );

//  void    setCrntNote( uint8_t nt ){ _crntNote = nt;}
//  uint8_t crntNote( void ) const { return _crntNote;}
//  void    setNowPlaying( bool on ){ _nowPlaying = on;}
  bool    nowPlaying( void ) const { return _nowPlaying;}
//  void    setMidiExp( uint8_t mexp ){ _midiExp = mexp;}
//  uint8_t midiExp( void ) const { return _midiExp;}
  uint8_t*  midiExpPtr( void ){ return &_midiExp;}

  static const unsigned char swTable[];

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

//  Voice Change
  int         _vceChangeProcess;  //  0:nothing 1->2->3->4:vceChange

};
#endif  /* MAGIC_FLUTE_H */
