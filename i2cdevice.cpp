/* ========================================
 *
 *	i2cdevice.cpp
 *		description: TouchMidi I2C Device Driver
 *
 *	Copyright(c)2018- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
*/
#include	"Arduino.h"
#include    <Wire.h>
#include	"configuration.h"
#include	"i2cdevice.h"

#include  "TouchMIDI_AVR_if.h"


//---------------------------------------------------------
//    Variables
//---------------------------------------------------------
int   i2cErrCode;

//---------------------------------------------------------
//		Initialize I2C Device
//---------------------------------------------------------
void wireBegin( void )
{
	Wire.begin();
  Wire.setClock(400000);
}
//---------------------------------------------------------
//		Write I2C Device
//    Err Code
//      0:success
//      1:data too long to fit in transmit buffer
//      2:received NACK on transmit of address
//      3:received NACK on transmit of data
//      4:other error
//---------------------------------------------------------
int write_i2cDevice( unsigned char adrs, unsigned char* buf, int count )
{
	Wire.beginTransmission(adrs);
  Wire.write(buf,count);
	return Wire.endTransmission();
}
//---------------------------------------------------------
//		Read 1byte I2C Device
//---------------------------------------------------------
int read1byte_i2cDevice( unsigned char adrs, unsigned char* wrBuf, unsigned char* rdBuf, int wrCount )
{
	unsigned char err;

	Wire.beginTransmission(adrs);
  Wire.write(wrBuf,wrCount);
	err = Wire.endTransmission(false);
	if ( err != 0 ){ return err; }

	err = Wire.requestFrom(adrs,(uint8_t)1,(uint8_t)0);
	while(Wire.available()) {
		*rdBuf = Wire.read();
	}

	err = Wire.endTransmission(true);
	return err;
}
//---------------------------------------------------------
//		Read N byte I2C Device
//---------------------------------------------------------
//送信結果 (byte) 
//0: 成功 
//1: 送ろうとしたデータが送信バッファのサイズを超えた 
//2: スレーブ・アドレスを送信し、NACKを受信した 
//3: データ・バイトを送信し、NACKを受信した 
//4: その他のエラー 
//
int read_nbyte_i2cDevice( unsigned char adrs, unsigned char* wrBuf, unsigned char* rdBuf, int wrCount, int rdCount )
{
	unsigned char err;

	Wire.beginTransmission(adrs);
  Wire.write(wrBuf,wrCount);
	err = Wire.endTransmission(false);
	if ( err != 0 ){ return err; }

	err = Wire.requestFrom(adrs,static_cast<uint8_t>(rdCount),(uint8_t)0);
	int rdAv = 0;
	while( rdAv = Wire.available() ) {
		*(rdBuf+rdCount-rdAv) = Wire.read();
	}

	err = Wire.endTransmission(true);
	return err;

	return 0;
}
//---------------------------------------------------------
//    Read Only N byte I2C Device
//    Err Code
//      0:success
//      1:data too long to fit in transmit buffer
//      2:received NACK on transmit of address
//      3:received NACK on transmit of data
//      4:other error
//---------------------------------------------------------
int read_only_nbyte_i2cDevice( unsigned char adrs, unsigned char* rdBuf, int rdCount )
{
  unsigned char err;

  err = Wire.requestFrom(adrs,static_cast<uint8_t>(rdCount),static_cast<uint8_t>(false));
  int rdAv = Wire.available();
  while( rdAv ) {
    *(rdBuf+rdCount-rdAv) = Wire.read();
    rdAv--;
  }

  err = Wire.endTransmission(true);
  return err;
}

#ifdef USE_CY8CMBR3110
//-------------------------------------------------------------------------
//			Cap Sense CY8CMBR3110 (Touch Sencer : I2c Device)
//-------------------------------------------------------------------------
#define		CONFIG_DATA_OFFSET	  0
#define		CONFIG_DATA_SZ			  128

#define		SENSOR_EN		          0x00	//	Register Address
#define		SENSITIVITY0			    0x08	//	Register Address
#define		SENSITIVITY1			    0x09	//	Register Address
#define		SENSITIVITY2			    0x0a	//	Register Address
#define		I2C_ADDR				      0x51	//	Register Address
#define		CONFIG_CRC				    0x7e	//	Register Address

#define		CTRL_CMD				      0x86	//	Register Address
#define		POWER_ON_AND_FINISHED	0x00
#define		SAVE_CHECK_CRC		    0x02
#define		DEVICE_RESET			    0xff

#define		CTRL_CMD_ERR			    0x89	//	Register Address

#define		FAMILY_ID_ADRS			  0x8f	//	Register Address
#define		FAMILY_ID				      0x9a
#define		DEVICE_ID_ADRS			  0x90	//	Register Address
#define		DEVICE_ID_LOW			    0x02
#define		DEVICE_ID_HIGH			  0x0a

#define		TOTAL_WORKING_SNS		  0x97	//	Register Address
#define		SNS_VDD_SHORT			    0x9a	//	Register Address
#define		SNS_GND_SHORT			    0x9c	//	Register Address
#define		BUTTON_STAT				    0xaa	//	Register Address

static const unsigned char CAP_SENSE_ADDRESS_ORG = 0x37;  //  Factory-Set
static const unsigned char CAP_SENSE_ADDRESS_1 = 0x38;
static const unsigned char CAP_SENSE_ADDRESS_2 = 0x39;

/*----------------------------------------------------------------------------*/
//
//      Write CY8CMBR3110 Config Data
//
/*----------------------------------------------------------------------------*/
#if 1 // wide range small resolution
/* Project: C:\Users\hasebems\Documents\Cypress Projects\Design0602\Design0602.cprj
 * Generated: 2019/06/02 6:52:53 +09:00 */
static const unsigned char tCY8CMBR3110_ConfigData[CONFIG_DATA_SZ] = {
    0xFFu, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0xFFu, 0x0Fu, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, //  adrs 08-0Ah: 11: 50count / 0.4pF
    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x02u, 0x01u, 0x08u,
    0x00u, 0x38u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xB7u, 0xCAu
};
/* Project: C:\Users\hasebems\Documents\Cypress Projects\Design0602\Design0602-2.cprj
 * Generated: 2019/06/02 7:07:15 +09:00 */
static const unsigned char tCY8CMBR3110_2_ConfigData[CONFIG_DATA_SZ] = {
    0xFFu, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0xFFu, 0x0Fu, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, //  adrs 08-0Ah: 11: 50count / 0.4pF
    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x02u, 0x01u, 0x08u,
    0x00u, 0x39u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x94u, 0x1Eu
};
#else // small range fine resolution
/* Generated: 2019/03/09 14:04:18 +09:00 */
static const unsigned char tCY8CMBR3110_ConfigData[CONFIG_DATA_SZ] =
{
    0xFFu, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, //  adrs 08-0Ah: 00: 50count / 0.1pF
    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x02u, 0x01u, 0x08u,
    0x00u, 0x38u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x81u, 0xA6u
};
/* Generated: 2019/03/09 14:11:51 +09:00 */
static const unsigned char tCY8CMBR3110_2_ConfigData[CONFIG_DATA_SZ] =
{
    0xFFu, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x80u, 0x80u, 0x80u, //  adrs 08-0Ah: 00: 50count / 0.1pF
    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x02u, 0x01u, 0x08u,
    0x00u, 0x39u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xA2u, 0x72u
};
#endif
//-------------------------------------------------------------------------
static const unsigned char* tConfigPtr[2] =
{
  tCY8CMBR3110_ConfigData,
  tCY8CMBR3110_2_ConfigData
};
//-------------------------------------------------------------------------
static const unsigned char tI2cAdrs[2] =
{
  CAP_SENSE_ADDRESS_1,
  CAP_SENSE_ADDRESS_2
};
//-------------------------------------------------------------------------
int MBR3110_init( int number )
{
 	unsigned char i2cdata[2];
	unsigned char selfCheckResult;

  const unsigned char* configData = tConfigPtr[number];
  unsigned char i2cAdrs = tI2cAdrs[number];

  delay(15);
	i2cdata[0] = CTRL_CMD;
  i2cdata[1] = POWER_ON_AND_FINISHED;
	write_i2cDevice(i2cAdrs,i2cdata,2);
  delay(900);

  unsigned char checksum1, checksum2;
  checksum1 = configData[126];
  checksum2 = configData[127];
  int err = MBR3110_checkWriteConfig(checksum1,checksum2,i2cAdrs);
  if ( err != 0 ){
    return err;
  }

  err = MBR3110_selfTest(&selfCheckResult,number);
  if ( err != 0 ){ return err; }

  if ( selfCheckResult & 0x80 ){
    err = selfCheckResult & 0x1f;  //  SENSOR_COUNT
    return err;
  }

  return err;
}
//-------------------------------------------------------------------------
int MBR3110_setup( int number )
{
  unsigned char i2cdata[2];
  unsigned char selfCheckResult;
  unsigned char i2cAdrs = 0;

  while(1){
#if ( FIRMMODE == WRITE_CNFG_FIRST_TIME_TO_MBR3110 )
    //  for factory preset device
    i2cAdrs = CAP_SENSE_ADDRESS_ORG;
#else
    //  for rewriting config to current device
    i2cAdrs = tI2cAdrs[number];
#endif

    delay(15);
    i2cdata[0] = CTRL_CMD;
    i2cdata[1] = POWER_ON_AND_FINISHED;
    write_i2cDevice(i2cAdrs,i2cdata,2);
    delay(900);
    int cnfgerr = MBR3110_writeConfig(number,i2cAdrs);
    
    i2cAdrs = tI2cAdrs[number];
    delay(15);
    i2cdata[0] = CTRL_CMD;
    i2cdata[1] = POWER_ON_AND_FINISHED;
    write_i2cDevice(i2cAdrs,i2cdata,2);
    delay(900);
    
    const unsigned char* configData = tConfigPtr[number];
    unsigned char checksum1, checksum2;
    checksum1 = configData[126];
    checksum2 = configData[127];      
    if ( MBR3110_checkWriteConfig(checksum1,checksum2,i2cAdrs) == 0 ){
      //  checksum got correct.
      break;
    }
  }

  int err = MBR3110_selfTest(&selfCheckResult,number);
  if ( err != 0 ){ return err; }

  if ( selfCheckResult & 0x80 ){
    err = selfCheckResult & 0x1f;  //  SENSOR_COUNT
    return err;
  }

  return err;
}
//-------------------------------------------------------------------------
//{
//  Change SENSITIVITY
//    if ( !IN1 ){
//      if ( !IN2 ){
//        MBR3110_changeSensitivity(3); //  Sens MAX (0.4pF)
//      }
//      else {
//        MBR3110_changeSensitivity(1); //  0.2pF
//      }
//    }
//    else {
//      if ( !IN2 ){
//        MBR3110_changeSensitivity(2); //  0.3pF
//      }
//    }
//}
//-------------------------------------------------------------------------
int MBR3110_readData( unsigned char cmd, unsigned char* data, int length, unsigned char i2cAdrs )
{
	int err;
	volatile int cnt = 0;
	unsigned char wrtBuf = cmd;

	while(1) {
		err = read_nbyte_i2cDevice(i2cAdrs,&wrtBuf,data,1,length);
		if ( err == 0 ) break;
		if ( ++cnt > 500 ){	//	if more than 500msec, give up and throw err
			return err;
		}
		delay(1);
	}
	return 0;
}
//-------------------------------------------------------------------------
int MBR3110_selfTest( unsigned char* result, int number )
{
	int err;

	err = MBR3110_readData(TOTAL_WORKING_SNS,result,1,tI2cAdrs[number]);
	if ( err ){ return err; }

	return 0;
}
//-------------------------------------------------------------------------
void MBR3110_changeSensitivity( unsigned char data, int number )		//	data : 0-3
{
	unsigned char i2cdata[2];
	unsigned char regData2 = data & 0x03;
	regData2 |= (regData2 << 2);
	unsigned char regData4 = regData2 | (regData2<<4);
  unsigned char i2cAdrs = tI2cAdrs[number];  

	i2cdata[0] = SENSITIVITY0;
	i2cdata[1] = regData4;
	if ( write_i2cDevice(i2cAdrs,i2cdata,2) ){
		i2cdata[0] = SENSITIVITY1;
		i2cdata[1] = regData4;
		if ( write_i2cDevice(i2cAdrs,i2cdata,2) ){
			i2cdata[0] = SENSITIVITY2;
			i2cdata[1] = regData2;
			if ( write_i2cDevice(i2cAdrs,i2cdata,2) ){
				return;
			}
		}
	}
}
//-------------------------------------------------------------------------
int MBR3110_readTouchSw( unsigned char* touchSw, int number )
{
  int err;

  err = MBR3110_readData(BUTTON_STAT,touchSw,2,tI2cAdrs[number]);
  if ( err ){ return err; }

  return 0;
}
//-------------------------------------------------------------------------
int MBR3110_checkWriteConfig( unsigned char checksumL, unsigned char checksumH, unsigned char crntI2cAdrs )
{
	unsigned char data[2];
	int err;

	err = MBR3110_readData(CONFIG_CRC,data,2,crntI2cAdrs);
	if ( err ){ return err; }

	//	err=0 means it's present config
	if (( data[0] == checksumL ) && ( data[1] == checksumH )){ return 0; }

	return -1;  //  check sum didn't match
}
//-------------------------------------------------------------------------
int MBR3110_writeConfig( int number, unsigned char crntI2cAdrs )
{
	unsigned char	data[CONFIG_DATA_SZ+1];
	int				err;
  const unsigned char* configData = tConfigPtr[number];

	//*** Step 1 ***
	//	Check Power On

	//	Check I2C Address
	err = MBR3110_readData(I2C_ADDR,data,1,crntI2cAdrs);
	if ( err != 0 ){ return err; }
	if ( data[0] != crntI2cAdrs ){ return -1; }

	//*** Step 2 ***
	err = MBR3110_readData(DEVICE_ID_ADRS,data,2,crntI2cAdrs);
	if ( err != 0 ){ return err; }
	if (( data[0] != DEVICE_ID_LOW ) || ( data[1] != DEVICE_ID_HIGH )){ return -2; }

	err = MBR3110_readData(FAMILY_ID_ADRS,data,1,crntI2cAdrs);
	if ( err != 0 ){ return err; }
	if ( data[0] != FAMILY_ID ){ return -3; }

	//*** Step 3 ***
	//	send Config Data
  for ( int i=0; i<CONFIG_DATA_SZ; i++ ){
    data[0] = CONFIG_DATA_OFFSET+i;
    data[1] = configData[i];
    err = write_i2cDevice(crntI2cAdrs,data,2);
    if ( err != 0 ){ return err;}
  }

	//	Write to flash
	data[0] = CTRL_CMD;
	data[1] = SAVE_CHECK_CRC;
	err = write_i2cDevice(crntI2cAdrs,data,2);
	if ( err != 0 ){ return err;}

	//	220msec Wait
	delay(300);

	//	Check to finish writing
	unsigned char wrtData = CTRL_CMD_ERR;
	err = read1byte_i2cDevice(crntI2cAdrs,&wrtData,data,1);
	if ( data[0] == 0xfe ){ return -4;}       //  bad check sum
  else if ( data[0] == 0xff ){ return -5;}  //  invalid command
  else if ( data[0] == 0xfd ){ return -6;}  //  failed to write flash

	//	Reset
	data[0] = CTRL_CMD;
	data[1] = DEVICE_RESET;
	err = write_i2cDevice(crntI2cAdrs,data,2);
	if ( err != 0 ){ return err;}

	//	100msec Wait
  delay(100);

	//*** Step 4 ***
	//	Get Config Data
	wrtData = CONFIG_DATA_OFFSET;
	err =  read_nbyte_i2cDevice(crntI2cAdrs,&wrtData,data,1,CONFIG_DATA_SZ);
	if ( err != 0 ){ return err; }

	//	Compare both Data
	for ( int i=0; i<CONFIG_DATA_SZ; i++ ){
		if ( configData[i] != data[i] ){ return /*data[i]*/i; }
	}
	
	return 0;
}
#endif



#ifdef USE_ADA88
//---------------------------------------------------------
//		<< ADA88 >>
//---------------------------------------------------------
static const unsigned char ADA88_I2C_ADRS = 0x70;
//---------------------------------------------------------
//		Initialize ADA88 LED Matrix
//---------------------------------------------------------
void ada88_init( void )
{
	unsigned char i2cBuf[2] = {0};
	i2cBuf[0] = 0x21;
	write_i2cDevice( ADA88_I2C_ADRS, i2cBuf, 2 );

	i2cBuf[0] = 0x81;
	write_i2cDevice( ADA88_I2C_ADRS, i2cBuf, 2 );

	i2cBuf[0] = 0xef;
	write_i2cDevice( ADA88_I2C_ADRS, i2cBuf, 2 );

	ada88_write(0);
}
//---------------------------------------------------------
//	write Character to Ada LED-matrix
//---------------------------------------------------------
void ada88_write( int letter )
{
	int	i;
	unsigned char i2cBufx[17];
	static const unsigned char letters[21][8] = {
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	//	0:nothing
		{0x02,0x05,0x88,0x88,0x8f,0x88,0x88,0x88},	//	1:A
		{0x87,0x88,0x88,0x87,0x88,0x88,0x88,0x87},	//	2:B
		{0x07,0x88,0x88,0x80,0x80,0x88,0x88,0x07},	//	3:C
		{0x87,0x88,0x88,0x88,0x88,0x88,0x88,0x87},	//	4:D
		{0x87,0x80,0x80,0x87,0x80,0x80,0x80,0x8f},	//	5:E
		{0x87,0x80,0x80,0x87,0x80,0x80,0x80,0x80},	//	6:F
		{0x07,0x88,0x80,0x80,0x8e,0x88,0x88,0x07},	//	7:G
		{0x88,0x88,0x88,0x8f,0x88,0x88,0x88,0x88},	//	8:H

		{0x02,0x05,0x88,0xe8,0xaf,0xe8,0xa8,0xa8},	//	9:AF
		{0x87,0x88,0x88,0xe7,0xa8,0xe8,0xa8,0xa7},	//	10:BF
		{0x07,0x88,0x88,0xe0,0xa0,0xe8,0xa8,0x27},	//	11:CF
		{0x87,0x88,0x88,0xe8,0xa8,0xe8,0xa8,0xa7},	//	12:DF
		{0x87,0x80,0x80,0xe7,0xa0,0xe0,0xa0,0xaf},	//	13:EF
		{0x87,0x80,0x80,0xe7,0xa0,0xe0,0xa0,0xa0},	//	14:FF
		{0x07,0x88,0x80,0xe0,0xae,0xe8,0xa8,0x27},	//	15:GF

		{0x97,0x90,0x90,0x97,0x90,0x90,0x90,0xd0},	//	16:Fl.
		{0x13,0x94,0x94,0xf4,0xd4,0xd4,0xb4,0x53},	//	17:Ob.

		{0x04,0x1f,0x04,0x1f,0x04,0x0f,0x15,0x22},	//	18:MA
		{0x04,0x0f,0x04,0x1e,0x08,0x12,0x01,0x0e},	//	19:KI
		{0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa}	//	20:
	};

	i2cBufx[0] = 0;
	for ( i=0; i<8; i++ ){
		i2cBufx[i*2+1] = letters[letter][i];
		i2cBufx[i*2+2] = 0;
	}
	write_i2cDevice( ADA88_I2C_ADRS, i2cBufx, 17 );
}
//---------------------------------------------------------
void ada88_writeNumber( int num )	//	num 1999 .. -1999
{
	int i;
	unsigned char i2cBufx[17];
	unsigned char ledPtn[8] = {0};
	static const unsigned char numletter[10][5] = {
		{ 0x07, 0x05, 0x05, 0x05, 0x07 },
		{ 0x04, 0x04, 0x04, 0x04, 0x04 },
		{ 0x07, 0x04, 0x07, 0x01, 0x07 },
		{ 0x07, 0x04, 0x07, 0x04, 0x07 },
		{ 0x05, 0x05, 0x07, 0x04, 0x04 },
		{ 0x07, 0x01, 0x07, 0x04, 0x07 },
		{ 0x07, 0x01, 0x07, 0x05, 0x07 },
		{ 0x07, 0x04, 0x04, 0x04, 0x04 },
		{ 0x07, 0x05, 0x07, 0x05, 0x07 },
		{ 0x07, 0x05, 0x07, 0x04, 0x07 }
	};
	static const unsigned char graph[10][2] = {
		{ 0x00, 0x00 },
		{ 0x00, 0x40 },
		{ 0x40, 0x60 },
		{ 0x60, 0x70 },
		{ 0x70, 0x78 },
		{ 0x78, 0x7c },
		{ 0x7c, 0x7e },
		{ 0x7e, 0x7f },
		{ 0x7f, 0xff },
		{ 0xff, 0xff }
	};

	if ( num > 1999 ){ num = 1999; }
	else if ( num < -1999 ){ num = -1999;}

	//	+/-, over 1000 or not
	if ( num/1000 ){ ledPtn[5] |= 0x80; }
	if ( num<0 ){
		ledPtn[2] |= 0x80;
		num = -num;
	}

	int num3digits = num%1000;
	int hundred = num3digits/100;
	int num2degits = num3digits%100;
	int deci = num2degits/10;
	int z2n = num2degits%10;

	for ( i=0; i<5; i++ ){
		ledPtn[i] |= numletter[hundred][i];
	}
	for ( i=0; i<5; i++ ){
		ledPtn[i] |= (numletter[deci][i]<<4);
	}
	for ( i=0; i<2; i++ ){
		ledPtn[i+6] = graph[z2n][i];
	}

	i2cBufx[0] = 0;
	for ( i=0; i<8; i++ ){
		i2cBufx[i*2+1] = ledPtn[i];
		i2cBufx[i*2+2] = 0;
	}
	write_i2cDevice( ADA88_I2C_ADRS, i2cBufx, 17 );
}
#endif


#ifdef USE_AP4
//---------------------------------------------------------
//    << AP4 >>
//---------------------------------------------------------
#define   AP4_I2C_ADRS 0x28
//-------------------------------------------------------------------------
//      AP4
//-------------------------------------------------------------------------
int ap4_getAirPressure( void )
{
  int   err = 0;
  unsigned char buf[2];
  err = read_only_nbyte_i2cDevice( AP4_I2C_ADRS, buf, 2);
  return (static_cast<int>(buf[0]&0x3f)*256+buf[1])/10;
}
#endif


#ifdef USE_AQM1602XA
//---------------------------------------------------------
//		<< AQM1602XA >>		I2C freq. is less than 100[kHz]
//---------------------------------------------------------
#define		AQM1602XA_I2C_ADRS	0x3e
//---------------------------------------------------------
//		Initialize AQM0802A
//---------------------------------------------------------
void aqm1602xa_init( void )
{
	unsigned char	i2cBuf[2];
	int	err;

	CyDelay(40);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x38;
	err = write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x39;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x14;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x73;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x56;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x6a;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(200);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x38;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x01;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(20);
	i2cBuf[0] = 0x00; i2cBuf[1] = 0x0c;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
	if ( err ){ i2cErrCode = err; return; }

	CyDelay(2);
}
//---------------------------------------------------------
//		write AQM0802A
//---------------------------------------------------------
void aqm1602xa_setStringUpper( int locate, char* str, int strNum )
{
	unsigned char	i2cBuf[2];
	int		i;

	if ( locate >= 8 ) return;
	if ( locate + strNum > 8 ){ strNum = 8 - locate; }

	i2cBuf[0] = 0x00; i2cBuf[1] = (uint8)locate | 0x80;
	write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
    CyDelayUs(500);		//	needs more than this period

	for ( i=0; i<strNum; i++ ){
		i2cBuf[0] = 0x40; i2cBuf[1] = str[i];
		write_i2cDevice( AQM1602XA_I2C_ADRS, i2cBuf, 2 );
		CyDelayUs(500);//	needs more than this period
	}

}
#endif


#ifdef USE_ADXL345
//---------------------------------------------------------
//		<< ADXL345 >>
//---------------------------------------------------------
#define		ADXL345_I2C_ADRS	0x1d
#define		ADXL345_I2C_ADRS2	0x53
//-------------------------------------------------------------------------
//			ADXL345 (Acceleration Sencer : I2c Device)
//-------------------------------------------------------------------------
//	for Acceleration Sencer
#define	ACCEL_SNCR_RATE				0x2c
#define ACCEL_SNCR_PWR_CTRL			0x2d
#define ACCEL_SNCR_DATA_FORMAT		0x31
//-------------------------------------------------------------------------
void adxl345_init( unsigned char chipnum )
{
	unsigned char i2cadrs = ADXL345_I2C_ADRS;
	if ( chipnum == 1 ){ i2cadrs = ADXL345_I2C_ADRS2; }

	uint8	i2cBuf[2];

	//	Start Access
	i2cBuf[0] = ACCEL_SNCR_PWR_CTRL; i2cBuf[1] = 0x08;
	write_i2cDevice(i2cadrs,i2cBuf,2);			//	Start Measurement
	i2cBuf[0] = ACCEL_SNCR_DATA_FORMAT; i2cBuf[1] = 0x04;
	write_i2cDevice(i2cadrs,i2cBuf,2 );		//	Left Justified, 2g

#if 0	//	if Shaker
	write_i2cDevice(i2cadrs,ACCEL_SNCR_RATE,0x0b);				//	200Hz (5msec)
	write_i2cDevice(i2cadrs,ACCEL_SNCR_PWR_CTRL,0x08);			//	Start Measurement
	write_i2cDevice(i2cadrs,ACCEL_SNCR_DATA_FORMAT,0x05);		//	Left Justified, 4g
#endif
}
//-------------------------------------------------------------------------
int adxl345_getAccel( unsigned char chipnum, signed short* value )
{
	unsigned short tmp;
	unsigned char reg[2];
	unsigned char adrs;
	unsigned char i2cadrs = ADXL345_I2C_ADRS;
	int err;

	if ( chipnum == 1 ){ i2cadrs = ADXL345_I2C_ADRS2; }

	adrs = 0x32;
	err = read_nbyte_i2cDevice( i2cadrs, &adrs, reg, 1, 2 );
	if (!err){
		tmp = reg[0];
		tmp |= (unsigned short)reg[1] << 8;
		*value = (signed short)tmp;
	}
	else {
		*value = 0;
	}

	adrs = 0x34;
	err = read_nbyte_i2cDevice( i2cadrs, &adrs, reg, 1, 2 );
	if (!err){
		tmp = reg[0];
		tmp |= (unsigned short)reg[1] << 8;
		*(value+1) = (signed short)tmp;
	}
	else {
		*(value+1) = 0;
	}

	adrs = 0x36;
	err = read_nbyte_i2cDevice( i2cadrs, &adrs, reg, 1, 2 );
	if (!err){
		tmp = reg[0];
		tmp |= (unsigned short)reg[1] << 8;
		*(value+2) = (signed short)tmp;
	}
	else {
		*(value+2) = 0;
	}
	return 0;
}
#endif

//-------------------------------------------------------------------------
//			PCA9685 (LED Driver : I2c Device)
//-------------------------------------------------------------------------
#ifdef USE_PCA9685    //	for LED Driver
//-------------------------------------------------------------------------
int PCA9685_write( int chipNumber, uint8 cmd1, uint8 cmd2 )
{
	static const unsigned char PCA9685_ADDRESS = 0x40;
	unsigned char	i2cBuf[2];
	int		err = 0;
	i2cBuf[0] = cmd1; i2cBuf[1] = cmd2;
	err = write_i2cDevice( PCA9685_ADDRESS+chipNumber, i2cBuf, 2 );
	return err;
}
//-------------------------------------------------------------------------
//		Initialize
//-------------------------------------------------------------------------
void PCA9685_init( int chipNumber )
{
	//	Init Parameter
	PCA9685_write( chipNumber, 0x00, 0x00 );
	PCA9685_write( chipNumber, 0x01, 0x12 );//	Invert, OE=high-impedance
}
//-------------------------------------------------------------------------
//		rNum, gNum, bNum : 0 - 4094  bigger, brighter
//-------------------------------------------------------------------------
int PCA9685_setFullColorLED( int chipNumber, int ledNum, unsigned short* color  )
{
  int err = 0;
	int	i;

  ledNum &= 0x03;
	for ( i=0; i<3; i++ ){
		//	figure out PWM counter
		unsigned short colorCnt = *(color+i);
		colorCnt = 4095 - colorCnt;
		if ( colorCnt <= 0 ){ colorCnt = 1;}

		//	Set PWM On Timing
		err = PCA9685_write( chipNumber, (uint8)(0x06 + i*4 + ledNum*16), (uint8)(colorCnt & 0x00ff) );
        if ( err != 0 ){ return err; }
		err = PCA9685_write( chipNumber, (uint8)(0x07 + i*4 + ledNum*16), (uint8)((colorCnt & 0xff00)>>8) );
        if ( err != 0 ){ return err; }

		err = PCA9685_write( chipNumber, (uint8)(0x08 + i*4 + ledNum*16), 0 );
        if ( err != 0 ){ return err; }
		err = PCA9685_write( chipNumber, (uint8)(0x09 + i*4 + ledNum*16), 0 );
        if ( err != 0 ){ return err; }
	}
    return err;
}
#endif

#ifdef USE_ATTINY
//---------------------------------------------------------
//		<< ATtiny >>
//---------------------------------------------------------
#define		ATTINY_I2C_ADRS	0x40
//-------------------------------------------------------------------------
//			ATtiny ( LED drive : I2c Device)
//-------------------------------------------------------------------------
int attiny_setLed( unsigned char ledPtn )
{
	unsigned char	i2cBuf = ledPtn;
	int		err = 0;
	err = write_i2cDevice( ATTINY_I2C_ADRS, &i2cBuf, 1 );
	return err;
}
#endif


#ifdef USE_PCA9544A
//---------------------------------------------------------
//		<< PCA9544A >>
//---------------------------------------------------------
static const unsigned char PCA9544A_I2C_ADRS = 0x71; // A0:high
//-------------------------------------------------------------------------
//			PCA9544A ( I2C Multiplexer : I2c Device)
//-------------------------------------------------------------------------
int pca9544_changeI2cBus( int i2c )
{
	unsigned char	i2cBuf = 0x04 | (i2c&0x0003);
	int		err = 0;
	err = write_i2cDevice( PCA9544A_I2C_ADRS, &i2cBuf, 1 );
	return err;
}
#endif
/* [] END OF FILE */
