/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  air_pressure.cpp
 *    description: Air Pressure Class
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#ifndef AIR_PRESSURE_H
#define AIR_PRESSURE_H

#include <Arduino.h>

#define MOVING_AV_MAX 16

class AirPressure {

public:
  AirPressure( void ) : 
//    _lastRawPressure(0.0),
    _currentStandard(10000), _samePressureCounter(0),
    _lastMidiValue(0), _afterStartCounter(0),
    _movingAv(), _lastPressure(0) {}

  int   getPressure( void );
  bool  generateExpEvent( uint8_t* midiValue );

private:
  void      analyseStandardPressure( int crntPrs );
  uint8_t   interpolateMidiExp( uint8_t realExp );


  static const uint8_t  ZERO_OFFSET;
  static const int MIDI_EXP_ITP_STEP;
  static const int STABLE_COUNT;
  static const int PWRON_DEAD_BAND_TIME;
  static const int NOISE_WIDTH;

  static const int INPUT_INDEX_MAX = 130;
  static const uint8_t pressureToMidiTable[INPUT_INDEX_MAX];

//  float   _lastRawPressure;
  int     _currentStandard;
  int     _newProposedPressure;
  int     _samePressureCounter;
  uint8_t _lastMidiValue;
  int     _afterStartCounter;

  //  Moving Avarage for Air Pressure
  int     _movingAv[MOVING_AV_MAX];
  int     _lastPressure;
};
#endif

