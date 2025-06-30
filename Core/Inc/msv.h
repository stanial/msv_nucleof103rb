/*
 * msv.h
 *
 *  Created on: Dec 9, 2024
 *      Author: Stanislaus Albert Adiwijaya
 */

#ifndef INC_MSV_H_
#define INC_MSV_H_

#include "main.h"
#include "string.h"
#include <stdio.h>
#include <stdbool.h>
#include "adxl345.h"
#include "INA219.h"
#include "onewire.h"
#include "ds18b20.h"
#include "serial_monitor.h"
#include "A7670.h"
#include "gnss.h"

#define LED_Pin GPIO_PIN_5
#define PREF_SMS_STORAGE "\"SM\""
#define GMAPS_LINK "https://maps.google.com/?q="
#define LOW_BATTERY_THRESHOLD 6.8

extern I2C_HandleTypeDef hi2c1;

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void MSV_Init(void);
void MSV_Loop(void);

//void UART_SEND(UART_HandleTypeDef *huart, char buffer[]);
void StartUp(void);
void MSV_checkSystem(void);

/* ADXL345 */
void MSV_Accel_valueShow(void);
void MSV_Accel_rawValueShow(void);
void MSV_Accel_corrValueShow(void);

/* INA219 */
void MSV_PowerSens_ValueShow(void);

/* DS18B20 */
void MSV_TempSens_setEnable(bool Enable);
float MSV_TempSens_GetTemp(void);

/* Interrupt Handler */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void MSV_Subroutine1(void);
void MSV_Subroutine2(void);
void MSV_Subroutine3(void);

void MSV_setAlarm(void);
void MSV_getDateTime(char *time, char *date);
void MSV_setDateTime(NTP_datetime_t NTP);
uint8_t decimalToBCD(uint8_t decimal);
void MSV_showDateTime(void);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

void MSV_SMS_RX_Init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void MSV_updateThingSpeak(char Apikey[], float vbat, float temp, float lat, float lon);
void MSV_updateVBAT_ThingSpeak(char Apikey[], float vbat);
void MSV_updateTemp_ThingSpeak(char Apikey[], float temp);
void MSV_updatePos_ThingSpeak(char Apikey[], float lat, float lon);
//void A7670E_sendSMS(void);
//void getPos(void);

#endif /* INC_MSV_H_ */
