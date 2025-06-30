/*
 * A7670.h
 *
 *  Created on: Feb 12, 2025
 *      Author: Stanislaus Albert Adiwijaya
 */

#ifndef INC_A7670_H_
#define INC_A7670_H_

#include "main.h"
#include "string.h"
#include <stdio.h>

#include "serial_monitor.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

typedef struct datetime{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} NTP_datetime_t;

void A7670E_sendAT(char cmd[], uint32_t tx_timeout, uint32_t rx_timeout);

void A7670E_sendSMS(char mobileNumber[], char msg_content[]);

void A7670E_NETService_Init(void);
void A7670E_NETService_Term(void);

NTP_datetime_t A7670E_getDateTime(void);
void A7670E_SyncDateTime(void);

//void SIMTransmit(char *cmd);
//void mqttPublish(void);

#endif /* INC_A7670_H_ */
