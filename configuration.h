/* ========================================
 *
 *	configuration.h
 *		description: TouchMidi Configuration
 *
 *	Copyright(c)2017- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt.
 *
 * ========================================
*/
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//---------------------------------------------------------
//    Ocarina Setting
//---------------------------------------------------------
#define   USE_AIR_PRESSURE
#define   USE_SIX_TOUCH_SENS
#define   MAX_LED       6

//---------------------------------------------------------
//    Firmware Mode
//---------------------------------------------------------
#define   WRITE_CNFG_FIRST_TIME_TO_MBR3110  0
#define   WRITE_NEW_CNFG_SETTING            1
#define   NORMAL_MODE                       2
#define   FIRMMODE        NORMAL_MODE

//---------------------------------------------------------
//		I2C Device Configuration
//---------------------------------------------------------
#define		USE_CY8CMBR3110
#define		USE_ADA88
#define   USE_AP4
//#define		USE_LPS22HB
//#define		USE_LPS25H
//#define		USE_AQM1602XA
//#define		USE_ADXL345
//#define		USE_PCA9685
//#define		USE_ATTINY
//#define		USE_PCA9544A

#endif
