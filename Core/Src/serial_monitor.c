/*
 * serial_monitor.c
 *
 *  Created on: Feb 12, 2025
 *      Author: Stanislaus Albert Adiwijaya
 */

#include "serial_monitor.h"

void UART_SEND(UART_HandleTypeDef *huart, char buffer[])
{
   HAL_UART_Transmit(huart, (uint8_t*) buffer, strlen(buffer), HAL_MAX_DELAY);
}
