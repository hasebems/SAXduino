/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  magicflute.cpp
 *    description: magicflute
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#include "magicflute.h"
#include "TouchMIDI_AVR_if.h"
#include "configuration.h"
#include "i2cdevice.h"
#include  "air_pressure.h"

//-------------------------------------------------------------------------
//  Adjustable Value
#define     DEADBAND_POINT_TIME     6      //  ×10[msec]
//-------------------------------------------------------------------------
#define     OCT_SW      0x30
#define     CRO_SW      0x08
#define     SX_SW       0x07
#define     ALL_SW      (OCT_SW|CRO_SW|SX_SW)
#define     TAP_FLAG    0x80

//-------------------------------------------------------------------------
#ifdef USE_AIR_PRESSURE
AirPressure ap;
#endif

extern GlobalTimer gt;

//-------------------------------------------------------------------------
const unsigned char MagicFlute::swTable[64] = {

//   ooo   oox   oxo   oxx   xoo   xox   xxo   xxx  right hand (x:hold, o:open)
//  do(hi) so    fa    la    mi    ti    re    do
    0x24, 0x1f, 0x1d, 0x21, 0x1c, 0x23, 0x1a, 0x18,     //  ooo left hand
    0x25, 0x20, 0x1e, 0x20, 0x1b, 0x22, 0x1b, 0x19,     //  oox
    0x18, 0x13, 0x11, 0x15, 0x10, 0x17, 0x0e, 0x0c,     //  oxo
    0x19, 0x14, 0x12, 0x14, 0x0f, 0x16, 0x0f, 0x0d,     //  oxx
    0x18, 0x13, 0x11, 0x15, 0x10, 0x17, 0x0e, 0x0c,     //  xoo
    0x19, 0x14, 0x12, 0x14, 0x0f, 0x16, 0x0f, 0x0d,     //  xox
    0x0c, 0x07, 0x05, 0x09, 0x04, 0x0b, 0x02, 0x00,     //  xxo
    0x0d, 0x08, 0x06, 0x08, 0x03, 0x0a, 0x03, 0x01      //  xxx
};

/*----------------------------------------------------------------------------*/
//
//     Check Touch Sensor & Generate MIDI Event
//
/*----------------------------------------------------------------------------*/
void MagicFlute::checkSixTouch( void )
{
  uint8_t swb[2] = {};

#ifdef USE_CY8CMBR3110
  int err = MBR3110_readTouchSw(swb);
#endif
  if ( err == 0 ){
    _swState = ((uint16_t)swb[0]) | ((uint16_t)swb[1]<<8);
    uint8_t tch = 0;
    if ( swb[0] & 0x20 ){ tch |= 0x01;}
    if ( swb[0] & 0x10 ){ tch |= 0x02;}
    if ( swb[0] & 0x08 ){ tch |= 0x04;}
    if ( swb[0] & 0x04 ){ tch |= 0x08;}
    if ( swb[0] & 0x02 ){ tch |= 0x10;}
    if ( swb[0] & 0x01 ){ tch |= 0x20;}
    analyseSixTouchSens(tch);
  }
}
/*----------------------------------------------------------------------------*/
int MagicFlute::midiOutAirPressure( void )
{
  int prs = 0;
#ifdef USE_AIR_PRESSURE
  prs = ap.getPressure();
  if ( gt.timer10msecEvent() == true ){
    if ( ap.generateExpEvent(midiExpPtr()) == true ){
      if ( nowPlaying() == false ){
        if ( _midiExp > 0 ){
          _nowPlaying = true;
          setMidiBuffer( 0x90, _crntNote, 0x7f );
          _doremi = _crntNote%12;
        }
      }
      else {
        if ( _midiExp == 0 ){
          _nowPlaying = false;
          setMidiBuffer( 0x80, _crntNote, 0x40 );
          _doremi = 12;
        }
      }
      setMidiBuffer( 0xb0, 0x0b, _midiExp );
      setMidiBuffer( 0xb0, 0x01, (_midiExp>>2)+64 );
    }
  }
#endif
  return prs;
}
//-------------------------------------------------------------------------
void MagicFlute::periodic100msec( void )
{
  setNeoPixelExp( _doremi, _midiExp ); //  for debug
}
//-------------------------------------------------------------------------
void MagicFlute::setNewTouch( uint8_t tch )
{
  if ( _crntTouch != tch ){
    _lastTouch = _crntTouch;
    _crntTouch = tch;
  }
}
//-------------------------------------------------------------------------
uint8_t MagicFlute::getNewNote( void )
{
  _lastNote = swTable[_crntTouch & ALL_SW];
  _tapTouch = 0;
  _startTime = 0;
  _deadBand = 0;

  return _lastNote+60;
}
//-------------------------------------------------------------------------
bool MagicFlute::catchEventOfPeriodic( uint8_t& midiValue, uint32_t crntTime )
{
  bool    ret = false;
  midiValue = _lastNote;

  if ( _crntTouch == _lastTouch ){
    if ( _deadBand > 0 ){
      if ( _startTime != 0 ){
        if ( crntTime-_startTime > DEADBAND_POINT_TIME*_deadBand ){
          //  KeyOn
          midiValue = getNewNote();
          ret = true;
        }
      }
    }
  }

  else {
    if ((_deadBand > 0) && (_tapTouch&TAP_FLAG) && ((_tapTouch&ALL_SW) == _crntTouch)){
      //  KeyOn
      //if (_dbg) _dbg->printf("<<Tapped>>\n");
      midiValue = getNewNote();
      ret = true;
    }

    else {
      int diff;
      uint8_t newNote = swTable[_crntTouch & ALL_SW];

      if ( newNote > _lastNote ){ diff = newNote - _lastNote;}
      else { diff = _lastNote - newNote;}

      if ( diff >= 12 ){
        _startTime = crntTime;
        _deadBand = 3;
        if ((_crntTouch^_lastTouch)&_crntTouch){
          //if (_dbg) _dbg->printf("<<Set Tap>>\n");
          _tapTouch = _lastTouch|TAP_FLAG;
        }
      }
      else if ( diff >= 9 ){
        // 9 - 11
        _startTime = crntTime;
        _deadBand = 2;
      }
      else if ( diff >= 3 ){
        // 3 - 8
        _startTime = crntTime;
        _deadBand = 1;
      }
      else {
        // 0 - 2
        midiValue = getNewNote();
        ret = true;
      }
    }

    //  update lastSwData
    _lastTouch = _crntTouch;
  }

  return ret;
}
/*----------------------------------------------------------------------------*/
void MagicFlute::analyseSixTouchSens( uint8_t tch )
{
  _touchCurrentStatus = tch;
  setNewTouch(_touchCurrentStatus);

  if ( gt.timer10msecEvent() == true ){
    uint8_t mdNote;
    if ( catchEventOfPeriodic(mdNote, gt.timer10ms()) == true ){
      if ( _nowPlaying == true ){
        if ( mdNote != _crntNote ){
          setMidiBuffer( 0x90, mdNote, 0x7f );
          setMidiBuffer( 0x80, _crntNote, 0x40 );
        }
        else {
          setMidiBuffer( 0x80, _crntNote, 0x40 );          
          setMidiBuffer( 0x90, mdNote, 0x7f );
        }
        _doremi = mdNote%12;
      }
      else {
        setMidiBuffer(0xa0,mdNote,0x01);
        setMidiBuffer(0xa0, _crntNote, 0 );
      }
      _crntNote = mdNote;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MagicFlute::setNeoPixelExp( uint8_t note, uint8_t exprs )
{
#if 0
  //  expression reflects number of LED
  int blinkNum = (exprs >> 4) + 1;
  if ( exprs == 0 ){ blinkNum = 0;}

  for ( int i=0; i<MAX_LED; i++ ){
    if (( i<blinkNum ) /*& !( _swState & (0x0080>>i))*/){
      setLed( i, colorTbl(note%16,0), colorTbl(note%16,1), colorTbl(note%16,2));
    }
    else {
      setLed(i,0,0,0);
    }
  }
#else
  uint8_t light = (exprs >> 4) + 1;  // 1-8
  for ( int i=0; i<MAX_LED; i++ ){
    if ( _swState & (0x0001<<i)){
      int16_t red = colorTbl(note%16,0)*light/8;
      int16_t green = colorTbl(note%16,1)*light/8;
      int16_t blue = colorTbl(note%16,2)*light/8;
      setLed(i,red,green,blue);
    }
    else {
      setLed(i,0,0,0);
    }
  }
#endif
  lightLed();
}
