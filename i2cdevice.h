/* ========================================
 *
 *	i2cdevice.h
 *		description: TouchMidi I2C Device Driver
 *
 *	Copyright(c)2018- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
*/
#ifndef I2CDEVICE_H
#define I2CDEVICE_H

void wireBegin( void );
void initHardware( void );

// USE_CY8CMBR3110
	int MBR3110_init( int number=0 );
  int MBR3110_setup( int number=0 );
	int MBR3110_readData( unsigned char cmd, unsigned char* data, int length, unsigned char i2cAdrs );
	int MBR3110_selfTest( unsigned char* result, int number );
	void MBR3110_changeSensitivity( unsigned char data, int number=0 );
  int MBR3110_readTouchSw( unsigned char* touchSw, int number=0 );
	int MBR3110_checkWriteConfig( unsigned char checksumL, unsigned char checksumH, unsigned char crntI2cAdrs );
	int MBR3110_writeConfig( int number, unsigned char crntI2cAdrs );

// USE_ADA88
	void ada88_init( void );
	void ada88_write( int letter );
	void ada88_writeNumber( int num );

// USE_AQM1602XA
	void aqm1602xa_init( void );
	void aqm1602xa_setStringUpper( int locate, char* str, int strNum );

// USE_ADXL345
	void adxl345_init( unsigned char chipnum );
	int adxl345_getAccel( unsigned char chipnum, signed short* value );

// USE_PCA9685
	void PCA9685_init( int chipNumber );
	int PCA9685_setFullColorLED( int chipNumber, int ledNum, unsigned short* color  );

// USE_ATTINY
	int attiny_setLed( unsigned char ledPtn );

// USE_PCA9544A
	int pca9544_changeI2cBus( int i2c );

int write_i2cDevice( unsigned char adrs, unsigned char* buf, int count );
int read1byte_i2cDevice( unsigned char adrs, unsigned char* wrBuf, unsigned char* rdBuf, int wrCount );
int read_nbyte_i2cDevice( unsigned char adrs, unsigned char* wrBuf, unsigned char* rdBuf, int wrCount, int rdCount );

#endif
