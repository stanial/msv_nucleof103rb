/*
 * gnss.h
 *
 *  Created on: Feb 12, 2025
 *      Author: Stanislaus Albert Adiwijaya
 */

#ifndef INC_GNSS_H_
#define INC_GNSS_H_

#include "main.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#include "A7670.h"
#include "serial_monitor.h"

#define PREF_SMS_STORAGE "\"SM\""

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

typedef struct coordFloat {
    float lat;
    float lon;
} coord_t;

//void AT_Test(char cmd[], uint32_t tx_timeout, uint32_t rx_timeout);

//void sendSMS(void);
void GNSS_Init(uint32_t startup_time);
void GNSS_PowerDown();
void getPos(uint8_t rep);
char *getCharPos(char *rx_buffer, uint32_t buffer_size);
coord_t getPosition(void);

#endif /* INC_GNSS_H_ */
