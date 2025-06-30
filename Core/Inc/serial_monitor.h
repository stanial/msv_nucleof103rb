/*
 * serial_monitor.h
 *
 *  Created on: Feb 12, 2025
 *      Author: Stanislaus Albert Adiwijaya
 */

#ifndef INC_SERIAL_MONITOR_H_
#define INC_SERIAL_MONITOR_H_

#include "main.h"
#include "string.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;

void UART_SEND(UART_HandleTypeDef *huart, char buffer[]);

#endif /* INC_SERIAL_MONITOR_H_ */
