/*
 * adxl345.h
 *
 *  Created on: May 7, 2024
 *      Author: Stanislaus Albert Adiwijaya
 */
#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include "main.h"

// Adxl345 Device Address
#define adxl_address 0x53<<1
#define SCALE_FACTOR .0078

//struct AdxlCommands
//{
//	uint8_t	DEVID;	/*	Device ID	*/
//	uint8_t	THRESH_TAP;	/*	Tap threshold	*/
//	uint8_t	OFSX;	/*	X-axis offset	*/
//	uint8_t	OFSY;	/*	Y-axis offset	*/
//	uint8_t	OFSZ;	/*	Z-axis offset	*/
//	uint8_t	DUR;	/*	Tap duration	*/
//	uint8_t	Latent;	/*	Tap latency	*/
//	uint8_t	Window;	/*	Tap window	*/
//	uint8_t	THRESH_ACT;	/*	Activity threshold	*/
//	uint8_t	THRESH_INACT;	/*	Inactivity threshold	*/
//	uint8_t	TIME_INACT;	/*	Inactivity time	*/
//	uint8_t	ACT_INACT_CTL;	/*	Axis enable control for activity and inactivity detection	*/
//	uint8_t	THRESH_FF;	/*	Free-fall threshold	*/
//	uint8_t	TIME_FF;	/*	Free-fall time	*/
//	uint8_t	TAP_AXES;	/*	Axis control for single tap/double tap	*/
//	uint8_t	ACT_TAP_STATUS;	/*	Source of single tap/double tap	*/
//	uint8_t	BW_RATE;	/*	Data rate and power mode control	*/
//	uint8_t	POWER_CTL;	/*	Power-saving features control	*/
//	uint8_t	INT_ENABLE;	/*	Interrupt enable control	*/
//	uint8_t	INT_MAP;	/*	Interrupt mapping control	*/
//	uint8_t	INT_SOURCE;	/*	Source of interrupts	*/
//	uint8_t	DATA_FORMAT;	/*	Data format control	*/
//	uint8_t	DATAX0;	/*	X-Axis Data 0	*/
//	uint8_t	DATAX1;	/*	X-Axis Data 1	*/
//	uint8_t	DATAY0;	/*	Y-Axis Data 0	*/
//	uint8_t	DATAY1;	/*	Y-Axis Data 1	*/
//	uint8_t	DATAZ0;	/*	Z-Axis Data 0	*/
//	uint8_t	DATAZ1;	/*	Z-Axis Data 1	*/
//	uint8_t	FIFO_CTL;	/*	FIFO control	*/
//	uint8_t	FIFO_STATUS;	/*	FIFO status	*/
//};
//
//struct AdxlCommands AdxlReg = {0x00, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
//	0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
//	0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

// Set i2c Handler here
extern I2C_HandleTypeDef hi2c1;

typedef struct xyzFloat {
    float x;
    float y;
    float z;
} accel;

typedef struct xyzRawint {
	int16_t x;
	int16_t y;
	int16_t z;
} accraw;

void adxl_write(uint8_t reg, uint8_t value);
void adxl_read_values(uint8_t reg);
void adxl_read_address(uint8_t reg);
void adxl_init(void);
void adxl_StandBy(void);
void adxl_int_act_setup(void);
void adxl_int_clear(void);

accraw getRaw(void);
accel getCorrected(void);

#endif /* INC_ADXL345_H_ */
