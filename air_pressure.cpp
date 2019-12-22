/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  air_pressure.cpp
 *    description: Air Pressure Functions
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#include "air_pressure.h"
#include "configuration.h"
#include "i2cdevice.h"
#include "TouchMIDI_AVR_if.h"


const uint8_t AirPressure::pressureToMidiTable[130] =
{
  0,18,29,36,42,47,51,54,57,60,
  62,65,67,69,70,72,74,75,77,78,
  79,80,82,83,84,85,86,87,88,88,
  89,90,91,92,92,93,94,95,95,96,
  97,97,98,98,99,100,100,101,101,102,
  102,103,103,104,104,105,105,106,106,106,
  107,107,108,108,109,109,109,110,110,110,
  111,111,112,112,112,113,113,113,114,114,
  114,115,115,115,116,116,116,116,117,117,
  117,118,118,118,118,119,119,119,119,120,
  120,120,121,121,121,121,121,122,122,122,
  122,123,123,123,123,124,124,124,124,124,
  125,125,125,125,126,126,126,126,126,127
};

/*----------------------------------------------------------------------------*/
const uint8_t  AirPressure::ZERO_OFFSET = 8;
const int AirPressure::MIDI_EXP_ITP_STEP = 8;
const int AirPressure::STABLE_COUNT = 200;          // *10msec = 2sec
const int AirPressure::PWRON_DEAD_BAND_TIME = 120;  // *10msec = 1.2sec
const int AirPressure::NOISE_WIDTH = 5;

/*----------------------------------------------------------------------------*/
//
//     Get Air Presure
//
/*----------------------------------------------------------------------------*/
int AirPressure::getPressure( void )
{
  //  Pressure Sensor Moving Avarage
  for ( int i=0; i<MOVING_AV_MAX-1; i++ ){
    _movingAv[i] = _movingAv[i+1];
  }
  _movingAv[MOVING_AV_MAX-1] = ap4_getAirPressure(); // analogDataRead();

  int total = 0;
  for ( int i=0; i<MOVING_AV_MAX; i++ ){
    total += _movingAv[i];
  }
  _lastPressure = total/MOVING_AV_MAX;
  return _lastPressure;
}
/*----------------------------------------------------------------------------*/
//
//     Generate MIDI Event
//
/*----------------------------------------------------------------------------*/
bool AirPressure::generateExpEvent( uint8_t* midiValue )
{
    bool    ret = false;

  if ( _afterStartCounter < PWRON_DEAD_BAND_TIME ){
    //  3sec. of the begining
    *midiValue = 0;
    _afterStartCounter++;
    _currentStandard = _lastPressure;
    return false;
  }

  int currentPrs = _lastPressure;

  //  Analyse & Generate Standard Pressure
  analyseStandardPressure(currentPrs);

  //  Generate MIDI Value
  int diff = currentPrs - _currentStandard;
  if ( diff < ZERO_OFFSET ){ diff = 0;}
  else if ( diff >= INPUT_INDEX_MAX + ZERO_OFFSET ){ diff = INPUT_INDEX_MAX-1;}
  else { diff -= ZERO_OFFSET;}

  uint8_t md = pressureToMidiTable[diff];
  uint8_t expr = _lastMidiValue;

  if ( md != _lastMidiValue ){
    //  interpolate MIDI value
    expr = interpolateMidiExp(md);
    _lastMidiValue = expr;
    ret = true;
  }

set_ret:
  //  output
  *midiValue = expr;
  return ret;
}
//-------------------------------------------------------------------------
void AirPressure::analyseStandardPressure( int crntPrs )
{
  //  not to mistake when blowing
  if ( _currentStandard+NOISE_WIDTH*2 < crntPrs ){
    _samePressureCounter = 0;   
    return;
  }

  //  Analize Standard Pressure
  if ( _newProposedPressure+NOISE_WIDTH < crntPrs ){
    _samePressureCounter = 0;
    _newProposedPressure = crntPrs - NOISE_WIDTH;
  }
  else if ( _newProposedPressure-NOISE_WIDTH > crntPrs ){
    _samePressureCounter = 0;
    _newProposedPressure = crntPrs + NOISE_WIDTH;
  }
  else {
    _samePressureCounter++;
    if ( _samePressureCounter > STABLE_COUNT ){
      _samePressureCounter = 0;
      if ( _currentStandard != _newProposedPressure ){
        //if (_dbg) _dbg->printf("<<Change Standard Pressure=%d>>\n",_newProposedPressure);
        _currentStandard = _newProposedPressure;
      }
    }
  }
}
//-------------------------------------------------------------------------
uint8_t AirPressure::interpolateMidiExp( uint8_t realExp )
{
  if ( realExp > _lastMidiValue ){
    if ( _lastMidiValue < 127 - MIDI_EXP_ITP_STEP ){
      if ( realExp > _lastMidiValue + MIDI_EXP_ITP_STEP ){
        realExp = _lastMidiValue + MIDI_EXP_ITP_STEP;
      }
    }
  }
  else {  // realExp < _lastMidiValue
    if ( _lastMidiValue > MIDI_EXP_ITP_STEP ){
      if ( realExp < _lastMidiValue - MIDI_EXP_ITP_STEP ){
        realExp = _lastMidiValue - MIDI_EXP_ITP_STEP;
      }
    }
  }
  //  never come when realExp == _lastMidiValue

  _lastMidiValue = realExp;
  return realExp;
}
