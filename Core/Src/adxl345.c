/*
 * adxl345.c
 *
 *  Created on: May 7, 2024
 *      Author: Stanislaus Albert Adiwijaya
 */
#include "adxl345.h"

uint8_t data_rec[6];
uint8_t chipid = 0;
uint8_t int_src;
//char x_char[3], y_char[3], z_char[3];

void adxl_write(uint8_t reg, uint8_t value)
{
	uint8_t data[2];
	data[0] = reg;
	data[1] = value;
	HAL_I2C_Master_Transmit(&hi2c1, adxl_address, data, 2, 100);
}

void adxl_read_values(uint8_t reg)
{
	HAL_I2C_Mem_Read(&hi2c1, adxl_address, reg, 1, (uint8_t *)data_rec, 6, 100);
}

void adxl_read_address(uint8_t reg)
{
	HAL_I2C_Mem_Read(&hi2c1, adxl_address, reg, 1, &chipid, 1, 100);
}

void adxl_init(void)
{
	uint8_t i2c_tx;
	adxl_read_address(0x00); // read the DEVID

//	adxl_write(0x31, 0x01);  // data_format range= +- 4g
	i2c_tx = ((1 << 5) | (1 << 0));
	adxl_write(0x31, i2c_tx);

	adxl_write(0x2d, 0x00);  // reset all bits
	adxl_write(0x2d, 0x0b);  // power_cntl measure and wake up 8hz
//	adxl_write(0x2c, 0x17);  // BW 12.5Hz Low-power mode
	adxl_write(0x2c, 0x07);
}

void adxl_StandBy(void)
{
	adxl_write(0x2d, 0x00);  // reset all bits
}

void adxl_int_act_setup(void)
{
	uint8_t i2c_tx[2];

	/* ACTIVITY */

	/* Activity Axes */
	adxl_write(0x27, 0x00);
	i2c_tx[0] = 0x27;
	i2c_tx[1] = ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4));
	HAL_I2C_Master_Transmit(&hi2c1, adxl_address, (uint8_t*)i2c_tx, 2, 100);

	/* Threshold Activity */
	adxl_write(0x24, 0x00);
	i2c_tx[0] = 0x24;
	i2c_tx[1] = 20;
	HAL_I2C_Master_Transmit(&hi2c1, adxl_address, (uint8_t*)i2c_tx, 2, 100);

	/* Int Map */
	adxl_write(0x2f, 0x00);
	i2c_tx[0] = 0x2f;
	i2c_tx[1] = ~(1 << 4);
	HAL_I2C_Master_Transmit(&hi2c1, adxl_address, (uint8_t*)i2c_tx, 2, 100);

	/* Interrupt Enable */
	adxl_write(0x2e, 0x00);
	i2c_tx[0] = 0x2e;
	i2c_tx[1] = (1 << 4);
	HAL_I2C_Master_Transmit(&hi2c1, adxl_address, (uint8_t*)i2c_tx, 2, 100);

	/* Measure */
	adxl_write(0x2d, 0x00);
	i2c_tx[0] = 0x2d;
//	i2c_tx[1] = ((1 << 3) | (1 << 2));
//	i2c_tx[1] = (1 << 3);
	i2c_tx[1] = 0x0b;
	HAL_I2C_Master_Transmit(&hi2c1, adxl_address, (uint8_t*)i2c_tx, 2, 100);
}

void adxl_int_clear(void)
{
	uint8_t int_src_reg = 0x30;
	HAL_I2C_Mem_Read(&hi2c1, adxl_address, int_src_reg, 1, &int_src, 1, 100);
//	int_src = 0;
}

accraw getRaw(void)
{
	accraw rawVal;
	adxl_read_values(0x32);
	rawVal.x = ((data_rec[1]<<8)|data_rec[0]);
	rawVal.y = ((data_rec[3]<<8)|data_rec[2]);
	rawVal.z = ((data_rec[5]<<8)|data_rec[4]);
	return rawVal;
}

accel getCorrected(void)
{
	accel corrVal = {0.0, 0.0, 0.0};
	accraw rawVal = getRaw();

	corrVal.x = rawVal.x * SCALE_FACTOR;
	corrVal.y = rawVal.y * SCALE_FACTOR;
	corrVal.z = rawVal.z * SCALE_FACTOR;
	return corrVal;
}
