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

#define     MAX_TONE_NUMBER     4
#define     MAX_TRANSPOSE       6
#define     MIN_TRANSPOSE       (-6)

//-------------------------------------------------------------------------
#ifdef USE_AIR_PRESSURE
AirPressure ap;
#endif

extern GlobalTimer gt;

//-------------------------------------------------------------------------
const unsigned char MagicFlute::swTable[64] = {

//   ooo   oox   oxo   oxx   xoo   xox   xxo   xxx  right hand (x:hold, o:open)
//  do(hi) so    fa    la    mi    ti    re    do
    0x60, 0x5b, 0x59, 0x5d, 0x58, 0x5f, 0x56, 0x54,     //  ooo left hand
    0x61, 0x5c, 0x5a, 0x5c, 0x57, 0x5e, 0x57, 0x55,     //  oox
    0x54, 0x4f, 0x4d, 0x51, 0x4c, 0x53, 0x4a, 0x48,     //  oxo
    0x55, 0x50, 0x4e, 0x50, 0x4b, 0x52, 0x4b, 0x49,     //  oxx
    0x54, 0x4f, 0x4d, 0x51, 0x4c, 0x53, 0x4a, 0x48,     //  xoo
    0x55, 0x50, 0x4e, 0x50, 0x4b, 0x52, 0x4b, 0x49,     //  xox
    0x48, 0x43, 0x41, 0x45, 0x40, 0x47, 0x3e, 0x3c,     //  xxo
    0x49, 0x44, 0x42, 0x44, 0x3f, 0x46, 0x3f, 0x3d,     //  xxx

// old
//    0x24, 0x1f, 0x1d, 0x21, 0x1c, 0x23, 0x1a, 0x18,     //  ooo left hand
//    0x25, 0x20, 0x1e, 0x20, 0x1b, 0x22, 0x1b, 0x19,     //  oox
//    0x18, 0x13, 0x11, 0x15, 0x10, 0x17, 0x0e, 0x0c,     //  oxo
//    0x19, 0x14, 0x12, 0x14, 0x0f, 0x16, 0x0f, 0x0d,     //  oxx
//    0x18, 0x13, 0x11, 0x15, 0x10, 0x17, 0x0e, 0x0c,     //  xoo
//    0x19, 0x14, 0x12, 0x14, 0x0f, 0x16, 0x0f, 0x0d,     //  xox
//    0x0c, 0x07, 0x05, 0x09, 0x04, 0x0b, 0x02, 0x00,     //  xxo
//    0x0d, 0x08, 0x06, 0x08, 0x03, 0x0a, 0x03, 0x01      //  xxx

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
    if ( _swState & 0x0020 ){ tch |= 0x01;}
    if ( _swState & 0x0010 ){ tch |= 0x02;}
    if ( _swState & 0x0008 ){ tch |= 0x04;}
    if ( _swState & 0x0004 ){ tch |= 0x08;}
    if ( _swState & 0x0002 ){ tch |= 0x10;}
    if ( _swState & 0x0001 ){ tch |= 0x20;}
    analyseSixTouchSens(tch);

    if ( nowPlaying() == false ){
      uint8_t newSwState = static_cast<uint8_t>((_swState & 0x03c0)>>6);
      if ( newSwState ^ _lastSwState ){
        //  Change Tone
        if ( newSwState & 0x01 ){
          if ( ++_toneNumber >= MAX_TONE_NUMBER ){ _toneNumber = 0; }
          setMidiBuffer( 0xc0, _toneNumber, 0xff );
          _ledIndicatorCntr = 1;
        }
        if ( newSwState & 0x02 ){
          if ( --_toneNumber < 0 ){ _toneNumber = MAX_TONE_NUMBER-1; }
          setMidiBuffer( 0xc0, _toneNumber, 0xff );
          _ledIndicatorCntr = 1;
        }
        //  Transpose
        if ( newSwState & 0x04 ){
          if ( ++_transpose > MAX_TRANSPOSE ){ _transpose = MAX_TRANSPOSE; }
          _ledIndicatorCntr = 101;
        }
        if ( newSwState & 0x08 ){
          if ( --_transpose < MIN_TRANSPOSE ){ _transpose = MIN_TRANSPOSE; }
          _ledIndicatorCntr = 101;
        }
      }
      _lastSwState = newSwState;
    }
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
      if (( nowPlaying() == false ) && ( _midiExp > 0 )){
        _nowPlaying = true;
        setMidiBuffer( 0x90, _crntNote+_transpose, 0x7f );
        _doremi = _crntNote%12;
      }
      else if (( nowPlaying() == true ) && ( _midiExp == 0 )){
        _nowPlaying = false;
        setMidiBuffer( 0x80, _crntNote+_transpose, 0x40 );
        _doremi = 12;
      }
      setMidiBuffer( 0xb0, 0x0b, _midiExp );
      setMidiBuffer( 0xb0, 0x01, (_midiExp>>3)+32 );
    }
  }
#endif
  return prs;
}
//-------------------------------------------------------------------------
void MagicFlute::periodic100msec( void )
{
  setNeoPixel();
}


/*----------------------------------------------------------------------------*/
//    private functions
/*----------------------------------------------------------------------------*/
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
  _lastSw = swTable[_crntTouch & ALL_SW];
  _tapTouch = 0;
  _startTime = 0;
  _deadBand = 0;

  return _lastSw;
}
//-------------------------------------------------------------------------
bool MagicFlute::decideDeadBand_byNoteDiff( uint8_t& midiValue, uint32_t crntTime, int diff )
{
  bool ret = false;

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

  return ret;
}
//-------------------------------------------------------------------------
bool MagicFlute::catchEventOfPeriodic( uint8_t& midiValue, uint32_t crntTime )
{
  bool    ret = false;

  if ( _crntTouch == _lastTouch ){
    if ( _deadBand > 0 ){
      if ( _startTime != 0 ){
        if ( crntTime-_startTime > DEADBAND_POINT_TIME*_deadBand ){
          //  NoteOn
          midiValue = getNewNote();
          ret = true;
        }
      }
    }
  }

  else {
    if (( _deadBand > 0 ) && ( _tapTouch&TAP_FLAG ) && (( _tapTouch&ALL_SW ) == _crntTouch )){
      //  NoteOn by Tap
      //if (_dbg) _dbg->printf("<<Tapped>>\n");
      midiValue = getNewNote();
      ret = true;
    }

    else {
      int diff;
      uint8_t newNote = swTable[_crntTouch & ALL_SW];

      if ( newNote > _lastSw ){ diff = newNote - _lastSw;}
      else { diff = _lastSw - newNote;}

      ret = decideDeadBand_byNoteDiff(midiValue, crntTime, diff);
    }

    //  update lastSwData
    _lastTouch = _crntTouch;
  }

  return ret;
}
/*----------------------------------------------------------------------------*/
void MagicFlute::analyseSixTouchSens( uint8_t tch )
{
  setNewTouch(tch);

  if ( gt.timer10msecEvent() == true ){
    uint8_t mdNote = _crntNote;
    if ( catchEventOfPeriodic(mdNote, gt.timer10ms()) == true ){
      if ( _nowPlaying == true ){
        if ( mdNote != _crntNote ){
          setMidiBuffer( 0x90, mdNote+_transpose, 0x7f );
          setMidiBuffer( 0x80, _crntNote+_transpose, 0x40 );
        }
        else {
          setMidiBuffer( 0x80, _crntNote+_transpose, 0x40 );          
          setMidiBuffer( 0x90, mdNote+_transpose, 0x7f );
        }
        _doremi = mdNote%12;
      }
      else {
        setMidiBuffer(0xa0, mdNote+_transpose, 0x01);
        setMidiBuffer(0xa0, _crntNote+_transpose, 0 );
      }
      _crntNote = mdNote;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MagicFlute::indicateParticularLed( int num, uint8_t red, uint8_t grn, uint8_t blu )
{
  for ( int i=0; i<_MAX_TOUCH_SW; ++i ){
    if ( i == num ){ setLed(i,red,grn,blu);}
    else { setLed(i,0,0,0);}
  }
}
/*----------------------------------------------------------------------------*/
void MagicFlute::indicateToneAndTranspose( void )
{
  if ( nowPlaying() == true ){
    _ledIndicatorCntr = 0;
  }
  else {
    if ( _ledIndicatorCntr > 100 ){
      //  Indicate Transpose
      ++_ledIndicatorCntr;
      if ( _ledIndicatorCntr > 103 ){
        indicateParticularLed(_ALL_CLEAR,0,0,0);
        _ledIndicatorCntr = 0;
      }
      else {
        const bool plus = (_transpose>=0)? true:false;
        const int ledNum = plus? _transpose:(-_transpose-1);
        if (plus == true) { indicateParticularLed(ledNum,100,0,0);}
        else              { indicateParticularLed(ledNum,0,0,100);}
      }
    }
    else {
      //  Indicate Tone Number
      ++_ledIndicatorCntr;
      if ( _ledIndicatorCntr > 3 ){
        indicateParticularLed(_ALL_CLEAR,0,0,0);
        _ledIndicatorCntr = 0;
      }
      else {
        indicateParticularLed(_toneNumber,0,100,0);
      }
    }
  }
}
/*----------------------------------------------------------------------------*/
void MagicFlute::indicatePitchAndExpression( void )
{
  uint8_t light = (_midiExp >> 4) + 1;  // 1-8
  for ( int i=0; i<MAX_LED; i++ ){
    if ( _swState & (0x0001<<i)){
      int16_t red = colorTbl(_doremi%16,0)*light/8;
      int16_t green = colorTbl(_doremi%16,1)*light/8;
      int16_t blue = colorTbl(_doremi%16,2)*light/8;
      setLed(i,red,green,blue);
    }
    else {
      setLed(i,0,0,0);
    }
  }
}
/*----------------------------------------------------------------------------*/
void MagicFlute::setNeoPixel( void )
{
#if 0
  //  expression reflects number of LED
  uint8_t note = _doremi;
  int blinkNum = (_exprs >> 4) + 1;
  if ( _exprs == 0 ){ blinkNum = 0;}

  for ( int i=0; i<MAX_LED; i++ ){
    if (( i<blinkNum ) /*& !( _swState & (0x0080>>i))*/){
      setLed( i, colorTbl(note%16,0), colorTbl(note%16,1), colorTbl(note%16,2));
    }
    else {
      setLed(i,0,0,0);
    }
  }

#else
  if ( _ledIndicatorCntr > 0 ){
    indicateToneAndTranspose();
  }
  else {
    indicatePitchAndExpression();
  }
#endif
  lightLed();
}
