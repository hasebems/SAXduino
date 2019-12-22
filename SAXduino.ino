/* ========================================
 *
 *  SAXduino.ino
 *    description: SAXduino Main Loop
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#include  <MsTimer2.h>
#include  <Adafruit_NeoPixel.h>
#include  "configuration.h"
#include  "TouchMIDI_AVR_if.h"

#include  "i2cdevice.h"
#include  "magicflute.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

/*----------------------------------------------------------------------------*/
//
//     Global Variables
//
/*----------------------------------------------------------------------------*/
#define NEO_PIXEL_PIN 2
Adafruit_NeoPixel led = Adafruit_NeoPixel(MAX_LED, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define RED_LED   6   //  LED for Debug
#define GREEN_LED 7   //  LED for Debug

/*----------------------------------------------------------------------------*/
GlobalTimer gt;
static MagicFlute mf;

/*----------------------------------------------------------------------------*/
//
//     Arduino Basic Functions
//
/*----------------------------------------------------------------------------*/
void flash()
{ //  interrupt
  gt.incGlobalTime();
}
/*----------------------------------------------------------------------------*/
void setup()
{
  //  Initialize Hardware
  wireBegin();
  Serial.begin(31250);

#ifdef USE_ADA88
  ada88_init();
  ada88_write(1);
#endif

  //  Set Port
  pinMode(5,OUTPUT);          //  Amp Mute
  digitalWrite(5,HIGH);
  pinMode(RED_LED, OUTPUT);   // LED
  digitalWrite(RED_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);   // LED
  digitalWrite(GREEN_LED, LOW);

  //  MBR3110
  //  first time only
#if (( FIRMMODE == WRITE_CNFG_FIRST_TIME_TO_MBR3110 ) || ( FIRMMODE == WRITE_NEW_CNFG_SETTING ))
  MBR3110_setup();
  while(1);
#else
  int err = MBR3110_init(); //  enable to omit an argument
  if ( err ){ while(1){ digitalWrite(RED_LED, HIGH); setAda88_Number(err);}}
#endif

  //  Set NeoPixel Library 
  led.begin();
  led.show(); // Initialize all pixels to 'off'

  //  Set Interrupt
  MsTimer2::set(10, flash);     // 10ms Interval Timer Interrupt
  MsTimer2::start();

  //  Opening
  for ( int i=0; i<6; i++ ){
    while( gt.globalTime() < (i+1)*10 ){
      setLed( i, 200, 180, 150 ); lightLed();
    }
    setLed( i, 0, 0, 0 );
    lightLed();
  }
}
/*----------------------------------------------------------------------------*/
void loop()
{
  //  Global Timer 
  generateTimer();

  //  Air Pressure Sensor
  int prs = mf.midiOutAirPressure();
  setAda88_Number(prs);

  //  Touch Sensor
  mf.checkSixTouch();

	delay(2);
}
/*----------------------------------------------------------------------------*/
//
//     Arduino API
//
/*----------------------------------------------------------------------------*/
int analogDataRead( void )
{
  return analogRead(0);
}
/*----------------------------------------------------------------------------*/
void displayError( void )
{
  digitalWrite(RED_LED, HIGH);
}
/*----------------------------------------------------------------------------*/
//
//     Global Timer
//
/*----------------------------------------------------------------------------*/
void generateTimer( void )
{
  uint32_t diff = gt.readGlobalTimeAndClear();

  gt.clearAllTimerEvent();
  gt.updateTimer(diff);
//  setAda88_Number(diff);

  if ( gt.timer100msecEvent() == true ){
    mf.periodic100msec();

    // blink LED
    (gt.timer100ms() & 0x0002)? digitalWrite(GREEN_LED, HIGH):digitalWrite(GREEN_LED, LOW);
  }
}
/*----------------------------------------------------------------------------*/
//
//     MIDI Command & UI
//
/*----------------------------------------------------------------------------*/
void setMidiBuffer( uint8_t dt0, uint8_t dt1, uint8_t dt2 )
{
  Serial.write(dt0);
  Serial.write(dt1);
  if ( dt2 != 0xff ) Serial.write(dt2);
}
/*----------------------------------------------------------------------------*/
void setMute( bool mute )
{
  if ( mute == true ){
    digitalWrite(5,HIGH);  //  Mute On
  }
  else {
    digitalWrite(5,LOW);  //  Mute Off    
  }
}
/*----------------------------------------------------------------------------*/
//
//     Hardware Access Functions
//
/*----------------------------------------------------------------------------*/
void setAda88_Number( int number )
{
#ifdef USE_ADA88
  ada88_writeNumber(number);  // -1999 - 1999
#endif
}
/*----------------------------------------------------------------------------*/
//
//     Blink LED by NeoPixel Library
//
/*----------------------------------------------------------------------------*/
const uint8_t colorTable[16][3] = {
  { 200,   0,   0 },//  C
  { 175,  30,   0 },
  { 155,  50,   0 },//  D
  { 135,  70,   0 },
  { 110,  90,   0 },//  E
  {   0, 160,   0 },//  F
  {   0, 100, 100 },
  {   0,   0, 250 },//  G
  {  30,   0, 230 },
  {  60,   0, 190 },//  A
  { 100,   0, 140 },
  { 140,   0,  80 },//  B

  { 100, 100, 100 },
  { 100, 100, 100 },
  { 100, 100, 100 },
  { 100, 100, 100 }
 };
/*----------------------------------------------------------------------------*/
uint8_t colorTbl( uint8_t doremi, uint8_t rgb ){ return colorTable[doremi][rgb];}
void setLed( int ledNum, uint8_t red, uint8_t green, uint8_t blue )
{
  led.setPixelColor(ledNum,led.Color(red, green, blue));
}
void lightLed( void )
{
  led.show();
}
