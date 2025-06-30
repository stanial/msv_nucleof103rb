/*
 * msv.c
 *
 *  Created on: Dec 9, 2024
 *      Author: Stanislaus Albert Adiwijaya
 */

#include "msv.h"

uint8_t ADXL_INT_FLAG = 0;
uint8_t RTC_INT_FLAG = 0;
uint8_t UART_INT_FLAG = 0;
uint8_t GET_POS_FLAG = 0;

char x_char[5], y_char[5], z_char[5];

uint8_t alarm_hh = 0;
uint8_t alarm_mm = 0;
uint8_t alarm_ss = 0;
uint8_t alarm_hh_incr = 0;
uint8_t alarm_mm_incr = 10;
uint8_t alarm_ss_incr = 0;

char timeData[15];
char dateData[15];

RTC_TimeTypeDef MSV_Time;
RTC_DateTypeDef MSV_Date;

INA219_t ina219;

//uint16_t vbus, vshunt;
//int16_t current;

float vbus, vshunt, current;
float ishunt;

char vb_char[20], vs_char[20], i_char[20], is_char[20];

//char fail_msg[0] = "Communication Failure!";

//float temperature;
char string[64];

char ATcommand[80];
uint8_t ATisOK = 0;
uint8_t slot = 0;
uint8_t rx_buffer[100] = {0};
uint8_t rx_index = 0;
uint8_t rx_data;

uint8_t Out1_state = 0;
uint8_t Out2_State = 0;

//char MobileNumber[] = "+491745749140";
char MobileNumber[] = "+4915735433359";

char Apikey[] = "LKUEFX435H1IMZS6";

float vbat_last, temp_last, lat_last, lon_last;

/*
 * Main Section
 */

/*
 * Initialization
 * This function will only executed only once at program start
 * */
void MSV_Init()
{
	StartUp();
	HAL_GPIO_WritePin(GPIOC, OUT2_EN_Pin, 1);
	adxl_init();
	INA219_Init(&ina219, &hi2c1, INA219_ADDRESS);
	HAL_Delay(10000);

	A7670E_SyncDateTime();
	A7670E_sendAT("AT+CTZU=1\r\n", 1000, 100);
	NTP_datetime_t current_datetime = A7670E_getDateTime();
	MSV_setDateTime(current_datetime);
	HAL_Delay(1000);

//	RTC_DateTypeDef cDate;
	RTC_TimeTypeDef cTime;

	// Get the RTC current Time
	HAL_RTC_GetTime(&hrtc, &cTime, RTC_FORMAT_BIN);
//	HAL_RTC_GetDate(&hrtc, &cDate, RTC_FORMAT_BIN);

//	alarm_hh = cTime.Hours;
//	alarm_mm = cTime.Minutes + 2;
//	alarm_ss = cTime.Seconds;

//	MSV_checkSystem();

	MSV_SMS_RX_Init();
//	A7670E_sendSMS();
//	GNSS_Init(10000);

	coord_t MSV_Pos = getPosition();
	char msg_str[150];

//	GNSS_PowerDown();
//	sprintf(msg_str,"System Started\r\nPosition: %s%0.7f,%0.7f \r\n", GMAPS_LINK, MSV_Pos.lat, MSV_Pos.lon);
//	UART_SEND(&huart2, msg_str);

//	// Send location via SMS
//	A7670E_sendSMS(MobileNumber, coord_str);

//	DS18B20_Enable();
	MSV_TempSens_setEnable(true);
	HAL_Delay(500);
	DS18B20_Init(DS18B20_Resolution_12bits);
	HAL_Delay(2000);
	float msv_temp = MSV_TempSens_GetTemp();
//	DS18B20_Disable();
	MSV_TempSens_setEnable(false);

	float msv_vbat = INA219_ReadBusVoltage(&ina219);
	INA219_Powerdown(&ina219);

	// Update ThingSpeak Channel
	MSV_updateThingSpeak(Apikey, msv_vbat, msv_temp, MSV_Pos.lat, MSV_Pos.lon);
	// Send location via SMS
	if (msv_vbat <= LOW_BATTERY_THRESHOLD)
	{
		sprintf(msg_str,"System Started but Battery Level is Low! (%0.2f V)\r\nPosition: %s%0.7f,%0.7f \r\n", msv_vbat, GMAPS_LINK, MSV_Pos.lat, MSV_Pos.lon);
	}
	else
	{
		sprintf(msg_str,"System Started\r\nPosition: %s%0.7f,%0.7f \r\n", GMAPS_LINK, MSV_Pos.lat, MSV_Pos.lon);
	}

	UART_SEND(&huart2, msg_str);
	A7670E_sendSMS(MobileNumber, msg_str);

	adxl_int_act_setup();
	adxl_int_clear();
	ADXL_INT_FLAG = 0;

	alarm_hh = cTime.Hours + alarm_hh_incr;
	alarm_mm = cTime.Minutes + alarm_mm_incr;
	alarm_ss = cTime.Seconds + alarm_ss_incr;

	MSV_setAlarm();

	vbat_last = msv_vbat;
	temp_last = msv_temp;
	lat_last = MSV_Pos.lat;
	lon_last = MSV_Pos.lon;

}

/*
 * Main Function
 * This function has to be called in the while loop
 * inside the main function of main.c
 * */
void MSV_Loop()
{
	UART_SEND(&huart2, "\r\n Currently in Sleep Mode, Waiting for Interrupts\r\n");
	HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_RESET);
//	adxl_int_clear();
	HAL_SuspendTick();
//	__enable_irq();
//	HAL_PWR_EnableSleepOnExit();
	HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
//	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_UART_Receive_IT(&huart1,&rx_data,1);
//	HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
//	__disable_irq();
//	HAL_NVIC_DisableIRQ(USART1_IRQn);
//	HAL_UART_AbortReceive(&huart1);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	UART_SEND(&huart2, " \r\n Waking up\r\n");
//	A7670E_sendAT("AT\r\n", 1000, 100);

	if(ADXL_INT_FLAG)
	{
		HAL_UART_AbortReceive(&huart1);
		MSV_Subroutine1();
		HAL_Delay(500);
		adxl_int_clear();
		HAL_Delay(1000);
		ADXL_INT_FLAG = 0;
	}

	if(RTC_INT_FLAG)
	{
		HAL_UART_AbortReceive(&huart1);
		UART_SEND(&huart2, " Woken up by RTC\r\n");
		MSV_Subroutine2();

		alarm_ss += alarm_ss_incr;

		if(alarm_ss>=60) {
			alarm_mm += 1;
			alarm_ss -= 60;
		}

		alarm_mm += alarm_mm_incr;

		if(alarm_mm>=60) {
			alarm_hh += 1;
			alarm_mm -= 60;
		}

		alarm_hh += alarm_hh_incr;

		if(alarm_hh>=24) alarm_hh -= 24;

		MSV_setAlarm();
		UART_SEND(&huart2, " Alarm set\r\n");

//		RTC_INT_FLAG = 0;
	}

	if(UART_INT_FLAG)
	{
//		HAL_UART_AbortReceive(&huart1);
		UART_SEND(&huart2, " Woken up from UART.\r\n");
		MSV_Subroutine3();
//		HAL_UART_AbortReceive(&huart1);
		UART_INT_FLAG = 0;
		GET_POS_FLAG = 0;
	}

//	MSV_Subroutine1();
//	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void StartUp()
{
	UART_SEND(&huart2, "\r\n");
	UART_SEND(&huart2, " =========================================== \r\n");
	UART_SEND(&huart2, " STM32 Program \r\n");
	HAL_Delay(500);
	UART_SEND(&huart2, " Hello from MSV!!! \r\n");
	HAL_Delay(500);
	UART_SEND(&huart2, " Starting");
	HAL_Delay(500);

	int i;
	for (i=0;i<3;i++)
	{
		UART_SEND(&huart2, ".");
		HAL_Delay(500);
	}

	UART_SEND(&huart2, "\r\n =========================================== \r\n ");
}

void MSV_checkSystem()
{
	MSV_SMS_RX_Init();

	coord_t MSV_Pos = getPosition();
	char msg_str[150];

	MSV_TempSens_setEnable(true);
	HAL_Delay(500);
	DS18B20_Init(DS18B20_Resolution_12bits);
	HAL_Delay(2000);
	float msv_temp = MSV_TempSens_GetTemp();
	MSV_TempSens_setEnable(false);

	float msv_vbat = INA219_ReadBusVoltage(&ina219);
	INA219_Powerdown(&ina219);

	// Update ThingSpeak Channel
	MSV_updateThingSpeak(Apikey, msv_vbat, msv_temp, MSV_Pos.lat, MSV_Pos.lon);
	// Send location via SMS
	if (msv_vbat <= LOW_BATTERY_THRESHOLD)
	{
		sprintf(msg_str,"Battery Level is Low: (%0.2f V)\r\nPosition: %s%0.7f,%0.7f \r\n", msv_vbat, GMAPS_LINK, MSV_Pos.lat, MSV_Pos.lon);
		UART_SEND(&huart2, msg_str);
		A7670E_sendSMS(MobileNumber, msg_str);
	}

}

/*
 *
 * ADXL345 Section
 *
 */
void MSV_Accel_ValueShow()
{
//	int16_t x, y, z;
	float accX, accY, accZ;

	accraw rawVal = getRaw();

	accX = rawVal.x * SCALE_FACTOR;
	accY = rawVal.y * SCALE_FACTOR;
	accZ = rawVal.z * SCALE_FACTOR;

	sprintf(x_char, "%4f", accX);
	UART_SEND(&huart2, " x = ");
	UART_SEND(&huart2, x_char);
	sprintf(y_char, "%4f", accY);
	UART_SEND(&huart2, "	y = ");
	UART_SEND(&huart2, y_char);
	sprintf(z_char, "%4f", accZ);
	UART_SEND(&huart2, "	z = ");
	UART_SEND(&huart2, z_char);
	UART_SEND(&huart2, "\r\n");

}

void MSV_Accel_rawValueShow()
{
	float accX, accY, accZ;
	accraw rawValues = getRaw();

	accX = rawValues.x * 1;
	accY = rawValues.y * 1;
	accZ = rawValues.z * 1;

	sprintf(x_char, "%4f", accX);
	UART_SEND(&huart2, " x = ");
	UART_SEND(&huart2, x_char);
	sprintf(y_char, "%4f", accY);
	UART_SEND(&huart2, "	y = ");
	UART_SEND(&huart2, y_char);
	sprintf(z_char, "%4f", accZ);
	UART_SEND(&huart2, "	z = ");
	UART_SEND(&huart2, z_char);
	UART_SEND(&huart2, "\r\n");

}

void MSV_Accel_corrValueShow()
{
	accel correctedValues = getCorrected();

	sprintf(x_char, "%.3f", correctedValues.x);
	UART_SEND(&huart2, " x = ");
	UART_SEND(&huart2, x_char);
	sprintf(y_char, "%.3f", correctedValues.y);
	UART_SEND(&huart2, "	y = ");
	UART_SEND(&huart2, y_char);
	sprintf(z_char, "%.3f", correctedValues.z);
	UART_SEND(&huart2, "	z = ");
	UART_SEND(&huart2, z_char);
	UART_SEND(&huart2, "\r\n");

}

/*
 *
 * INA219 Section
 *
 */
void MSV_PowerSens_ValueShow()
{
	vbus = INA219_ReadBusVoltage(&ina219);
	vshunt = INA219_ReadShuntVolage(&ina219);
	current = INA219_ReadCurrent(&ina219);
	ishunt = vshunt / 0.1;

	sprintf(vb_char, " VBus = %.2f V  ", vbus);
	UART_SEND(&huart2, vb_char);
	sprintf(vs_char, "VShunt = %.3f mV  ", vshunt);
	UART_SEND(&huart2, vs_char);
	sprintf(i_char, "I = %.1f mA  ", current);
	UART_SEND(&huart2, i_char);
	sprintf(is_char, "I_Estimate = %.1f mA\r\n", ishunt);
	UART_SEND(&huart2, is_char);
}

/*
 *
 * DS18B20 Section
 *
 */
void MSV_TempSens_setEnable(bool Enable)
{
	HAL_GPIO_WritePin(GPIOC, DS18B20_EN_Pin, Enable);
}

float MSV_TempSens_GetTemp()
{
	float temperature;
	DS18B20_ReadAll();
	DS18B20_StartAll();
	uint8_t ROM_tmp[8];
	uint8_t i;

	for(i = 0; i < DS18B20_Quantity(); i++)
	{
		if(DS18B20_GetTemperature(i, &temperature))
		{
			DS18B20_GetROM(i, ROM_tmp);
			memset(string, 0, sizeof(string));
			sprintf(string, "%d Temp = %.2f °C\r\n", i, temperature);
			UART_SEND(&huart2, string);
			return temperature;
		}
	}

//	return temperature;
}

/*
 * ISR
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
//	UART_SEND(&huart2, "\r\n Enter ISR");
	UART_SEND(&huart2, " Activity Detected!\r\n");
//	HAL_GPIO_TogglePin(GPIOA, LED_Pin);
	ADXL_INT_FLAG = 1;
//	UART_SEND(&huart2, "\r\n Exit ISR");
//	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
//	__disable_irq();
//	HAL_PWR_DisableSleepOnExit();
}

void MSV_Subroutine1()
{
	UART_SEND(&huart2, "\r\n ########################################### \r\n");
	UART_SEND(&huart2, " Routine 1 begin \r\n");

	HAL_UART_AbortReceive(&huart1);
	HAL_Delay(1000);

	MSV_SMS_RX_Init();

	char ATcommand[80] = "AT+CGNSSINFO\r\n";
	char buffer[86] = {0};

	float lat1, lon1;
	char latDir[2] = {0};
	char lonDir[2] = {0};

	GNSS_Init(10000);
	HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 10000);
	HAL_UART_Receive(&huart1, buffer, 100, 100);
	HAL_Delay(1000);
	GNSS_PowerDown();
	HAL_Delay(1000);

	const char s[2] = ",";
	char *token;
	char *ePtr;

	token = strtok(buffer, s);
	token = strtok(NULL, s);
	token = strtok(NULL, s);
	token = strtok(NULL, s);
	token = strtok(NULL, s);
	lat1 = strtof(token, &ePtr);
	token = strtok(NULL, s);
	strcpy(latDir, token);
	token = strtok(NULL, s);
	lon1 = strtof(token, &ePtr);
	token = strtok(NULL, s);
	strcpy(lonDir, token);

	if(strcmp(latDir, "S") == 0)
	{
		lat1 *= -1;
	}

	if(strcmp(lonDir, "W") == 0)
	{
		lon1 *= -1;
	}

//	coord_t MSV_Pos = getPosition();
	char act_msg_str[100];

//	GNSS_PowerDown();
//	HAL_Delay(1000);
//	A7670E_SMS_Init();
////	HAL_UART_Receive_IT(&huart1,&rx_data,1);
//	HAL_Delay(1000);
////	sprintf(act_msg_str,"Activity Detected!\r\nPosition: %s%0.7f,%0.7f \r\n", GMAPS_LINK, MSV_Pos.lat, MSV_Pos.lon);
	MSV_updateThingSpeak(Apikey, vbat_last, temp_last, lat1, lon1);
	sprintf(act_msg_str,"Activity Detected!\r\nPosition: %s%0.7f,%0.7f \r\n", GMAPS_LINK, lat1, lon1);
//	UART_SEND(&huart2, act_msg_str);
	A7670E_sendSMS(MobileNumber, act_msg_str);

	lat_last = lat1;
	lon_last = lon1;

	UART_SEND(&huart2, " Routine 1 ended \r\n");
	UART_SEND(&huart2, " ########################################### \r\n");
}

void MSV_Subroutine2()
{
	UART_SEND(&huart2, "\r\n ########################################### \r\n");
	UART_SEND(&huart2, " Routine 2 begin \r\n");

	MSV_showDateTime();
	INA219_Init(&ina219, &hi2c1, INA219_ADDRESS);
	MSV_checkSystem();
//	DS18B20_Enable();
//	HAL_Delay(500);
//	DS18B20_Init(DS18B20_Resolution_12bits);
//
//	float msv_vbat = INA219_ReadBusVoltage(&ina219);
//	INA219_Powerdown(&ina219);
//	HAL_Delay(5000);
//	float msv_temp = DS18B20_GetTemp();
//	HAL_Delay(500);
//	DS18B20_Disable();

//	int i;
//	int rep = 5;
//	for (i=0;i<rep;i++)
//	{
//		INA219_ValueShow();
//		HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_SET);
//		HAL_Delay(500);
//		HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_RESET);
//		HAL_Delay(500);
//	}
//
//	for (i=0;i<rep;i++)
//	{
//		DS18B20_GetTemp();
//		HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_SET);
//		HAL_Delay(500);
//		HAL_GPIO_WritePin(GPIOA,LED_Pin, GPIO_PIN_RESET);
//		HAL_Delay(500);
//	}

//	DS18B20_Disable();

//	A7670E_sendSMS();
//	A7670E_sendSMS(MobileNumber, "Battery Check\r\n");

//	MSV_updateThingSpeak(Apikey, msv_vbat, msv_temp, lat_last, lon_last);
//
//	if (msv_vbat <= LOW_BATTERY_THRESHOLD)
//	{
//		char msg_str[100];
//		sprintf(msg_str,"Battery Low!\r\nBattery Voltage = %0.2f\r\nPosition: %s%0.7f,%0.7f \r\n", msv_vbat, GMAPS_LINK, lat_last, lon_last);
//		UART_SEND(&huart2, msg_str);
//		A7670E_sendSMS(MobileNumber, msg_str);
//	}
//
//	vbat_last = msv_vbat;
//	temp_last = msv_temp;

	UART_SEND(&huart2, "\r\n Routine 2 ended \r\n");
	UART_SEND(&huart2, " ########################################### \r\n");
}

void MSV_Subroutine3()
{
	UART_SEND(&huart2, "\r\n ########################################### \r\n");
	UART_SEND(&huart2, " Routine 3 begin \r\n");

	if (Out1_state)
	{
		HAL_GPIO_WritePin(GPIOC, OUT1_EN_Pin, 1);
		UART_SEND(&huart2, " Output 1 ON \r\n");
	}

	if (!Out1_state)
	{
		HAL_GPIO_WritePin(GPIOC, OUT1_EN_Pin, 0);
		UART_SEND(&huart2, " Output 1 OFF \r\n");
	}

	if (Out2_State)
	{
		HAL_GPIO_WritePin(GPIOC, OUT2_EN_Pin, 0);
		UART_SEND(&huart2, " Output 2 ON \r\n");
	}

	if (!Out2_State)
	{
		HAL_GPIO_WritePin(GPIOC, OUT2_EN_Pin, 1);
		UART_SEND(&huart2, " Output 2 OFF \r\n");
	}

	if (GET_POS_FLAG)
	{
		UART_SEND(&huart2, " Retrieving Current Position \r\n");
		HAL_UART_AbortReceive(&huart1);

		char ATcommand[80] = "AT+CGNSSINFO\r\n";
		char buffer[86] = {0};

		float lat1, lon1;
		char latDir[2] = {0};
		char lonDir[2] = {0};

		GNSS_Init(10000);
		HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 10000);
		HAL_UART_Receive(&huart1, buffer, 100, 100);
		HAL_Delay(1000);
		GNSS_PowerDown();
		HAL_Delay(1000);

		const char s[2] = ",";
		char *token;
		char *ePtr;

		token = strtok(buffer, s);
		token = strtok(NULL, s);
		token = strtok(NULL, s);
		token = strtok(NULL, s);
		token = strtok(NULL, s);
		lat1 = strtof(token, &ePtr);
		token = strtok(NULL, s);
		strcpy(latDir, token);
		token = strtok(NULL, s);
		lon1 = strtof(token, &ePtr);
		token = strtok(NULL, s);
		strcpy(lonDir, token);

		if(strcmp(latDir, "S") == 0)
		{
			lat1 *= -1;
		}

		if(strcmp(lonDir, "W") == 0)
		{
			lon1 *= -1;
		}

		char req_msg_str[100];

		MSV_updateThingSpeak(Apikey, vbat_last, temp_last, lat1, lon1);
		sprintf(req_msg_str,"Requested Position: %s%0.7f,%0.7f \r\n", GMAPS_LINK, lat1, lon1);
		A7670E_sendSMS(MobileNumber, req_msg_str);

		lat_last = lat1;
		lon_last = lon1;

	}

	UART_SEND(&huart2, " Routine 3 ended \r\n");
	UART_SEND(&huart2, " ########################################### \r\n");
}

void MSV_setAlarm()
{
	RTC_AlarmTypeDef MSV_Alarm = {0};
	MSV_Alarm.AlarmTime.Hours = decimalToBCD(alarm_hh);
	MSV_Alarm.AlarmTime.Minutes = decimalToBCD(alarm_mm);
	MSV_Alarm.AlarmTime.Seconds = decimalToBCD(alarm_ss);
	MSV_Alarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &MSV_Alarm, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}
}

void MSV_getDateTime(char *time, char *date)
{
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	// Get the RTC current Time
	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	// Get the RTC current Date
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	// Display time Format: hh:mm:ss
	sprintf((char*)time,"%02d:%02d:%02d", gTime.Hours, gTime.Minutes, gTime.Seconds);

	// Display date Format: dd-mm-yyyy
//	sprintf((char*)date,"%s %02d-%02d-%2d", DayinWeek, gDate.Date, gDate.Month, 2000 + gDate.Year);
	sprintf((char*)date,"%02d-%02d-%2d", gDate.Date, gDate.Month, 2000 + gDate.Year);
}

void MSV_setDateTime(NTP_datetime_t NTP)
{
	MSV_Date.Date = decimalToBCD(NTP.day);
	MSV_Date.Month = decimalToBCD(NTP.month);
	MSV_Date.Year = decimalToBCD(NTP.year);
	MSV_Time.Hours = decimalToBCD(NTP.hours);
	MSV_Time.Minutes = decimalToBCD(NTP.minutes);
	MSV_Time.Seconds = decimalToBCD(NTP.seconds);
	HAL_RTC_SetTime(&hrtc, &MSV_Time, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &MSV_Date, RTC_FORMAT_BCD);
}

uint8_t decimalToBCD(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

void MSV_showDateTime()
{
	MSV_getDateTime(timeData, dateData);
	UART_SEND(&huart2, " Time = ");
	UART_SEND(&huart2, timeData);
	UART_SEND(&huart2, "	Date = ");
	UART_SEND(&huart2, dateData);
	UART_SEND(&huart2, "\r\n");
	HAL_Delay(1000);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	UART_SEND(&huart2, " Wake Up!\r\n");
	RTC_INT_FLAG = 1;
}

void MSV_SMS_RX_Init()
{
	while(!ATisOK)
	{
	    sprintf(ATcommand,"AT\r\n");
	    HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	    HAL_UART_Receive (&huart1, rx_buffer, 100, 100);
	    HAL_Delay(1000);
	    if(strstr((char *)rx_buffer,"OK"))
	    {
	        ATisOK = 1;
	    }
	    HAL_Delay(1000);
	    memset(rx_buffer,0,sizeof(rx_buffer));
	}
	    // Send AT+CMGF=1
	    sprintf(ATcommand,"AT+CMGF=1\r\n");
	    HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	    HAL_UART_Receive (&huart1, rx_buffer, 100, 100);
	    HAL_Delay(1000);
	    memset(rx_buffer,0,sizeof(rx_buffer));
	    // Send AT+CNMI=2,1 to enable notification when SMS arrives
	    sprintf(ATcommand,"AT+CNMI=2,1\r\n");
	    HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	    // Enabling interrupt receive
//	    HAL_UART_Receive_IT(&huart1,&rx_data,1);// receive data (one character only)
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	UART_INT_FLAG = 1;
  if(huart->Instance==USART1)
  {
    // if the character received is other than 'enter' ascii13, save the data in buffer
    if(rx_data!=13)
    {
      rx_buffer[rx_index++]=rx_data;
    }
    else
    {
      // if new message arrived, read the message
      if(sscanf((char*)rx_buffer, "\n+CMTI: " PREF_SMS_STORAGE ",%hhd", &slot)==1)
      {
    	  UART_INT_FLAG = 1;
        sprintf(ATcommand,"AT+CMGR=%d\r\n",slot);
        HAL_UART_Transmit_IT(&huart1,(uint8_t *)ATcommand,strlen(ATcommand));
      }
      if (strstr((char *)rx_buffer,"out1on"))
      {
    	  Out1_state = 1;
//        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 1);
      }
      if (strstr((char *)rx_buffer,"out1off"))
      {
    	  Out1_state = 0;
//        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 0);
      }
      if (strstr((char *)rx_buffer,"getpos"))
	  {
		  GET_POS_FLAG = 1;
//        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 0);
	  }
      if (strstr((char *)rx_buffer,"out2on"))
	  {
		  Out2_State = 1;
	  }
      if (strstr((char *)rx_buffer,"out2off"))
	  {
		  Out2_State = 0;
	  }
      // Delete all messages
      if (strstr((char *)rx_buffer,"READ"))
      {
          sprintf(ATcommand,"AT+CMGD=,4\r\n");
          HAL_UART_Transmit_IT(&huart1,(uint8_t *)ATcommand,strlen(ATcommand));
      }
      rx_index=0;
      memset(rx_buffer,0,sizeof(rx_buffer));
    }
    // Enabling UART interrupt receive again
    HAL_UART_Receive_IT(&huart1,&rx_data,1); // receive data
  }
}

void MSV_updateThingSpeak(char Apikey[], float vbat, float temp, float lat, float lon)
{
	A7670E_NETService_Init();
	A7670E_sendAT("AT+CIPOPEN=0,\"TCP\",\"api.thingspeak.com\",80\r\n", 1000, 100);

	A7670E_sendAT("AT+CIPSEND=0,150\r\n", 1000, 100);

	char http_str[150];
	sprintf(http_str,"GET https://api.thingspeak.com/update?api_key=%s&field1=%0.2f&field2=%0.2f&field3=%0.7f&field4=%0.7f\r\n",Apikey, vbat, temp, lat, lon);
	UART_SEND(&huart2, http_str);
//	A7670E_sendAT("GET https://api.thingspeak.com/update?api_key=LKUEFX435H1IMZS6&field1=5.0\r\n", 1000, 100);
	A7670E_sendAT(http_str, 1000, 100);
	A7670E_NETService_Term();

}

// To update channel field individually
void MSV_updateVBAT_ThingSpeak(char Apikey[], float vbat)
{
	A7670E_NETService_Init();
	A7670E_sendAT("AT+CIPOPEN=0,\"TCP\",\"api.thingspeak.com\",80\r\n", 1000, 100);

	A7670E_sendAT("AT+CIPSEND=0,150\r\n", 1000, 100);

	char http_str[150];
	sprintf(http_str,"GET https://api.thingspeak.com/update?api_key=%s&field1=%0.2f\r\n",Apikey, vbat);
	UART_SEND(&huart2, http_str);
//	A7670E_sendAT("GET https://api.thingspeak.com/update?api_key=LKUEFX435H1IMZS6&field1=5.0\r\n", 1000, 100);
	A7670E_sendAT(http_str, 1000, 100);
	A7670E_NETService_Term();

}

void MSV_updateTemp_ThingSpeak(char Apikey[], float temp)
{
	A7670E_NETService_Init();
	A7670E_sendAT("AT+CIPOPEN=0,\"TCP\",\"api.thingspeak.com\",80\r\n", 1000, 100);

	A7670E_sendAT("AT+CIPSEND=0,150\r\n", 1000, 100);

	char http_str[150];
	sprintf(http_str,"GET https://api.thingspeak.com/update?api_key=%s&field2=%0.2f\r\n",Apikey, temp);
	UART_SEND(&huart2, http_str);
//	A7670E_sendAT("GET https://api.thingspeak.com/update?api_key=LKUEFX435H1IMZS6&field1=5.0\r\n", 1000, 100);
	A7670E_sendAT(http_str, 1000, 100);
	A7670E_NETService_Term();

}

void MSV_updatePos_ThingSpeak(char Apikey[], float lat, float lon)
{
	A7670E_NETService_Init();
	A7670E_sendAT("AT+CIPOPEN=0,\"TCP\",\"api.thingspeak.com\",80\r\n", 1000, 100);

	A7670E_sendAT("AT+CIPSEND=0,150\r\n", 1000, 100);

	char http_str[150];
	sprintf(http_str,"GET https://api.thingspeak.com/update?api_key=%s&field3=%0.7f&field4=%0.7f\r\n",Apikey, lat, lon);
	UART_SEND(&huart2, http_str);
//	A7670E_sendAT("GET https://api.thingspeak.com/update?api_key=LKUEFX435H1IMZS6&field1=5.0\r\n", 1000, 100);
	A7670E_sendAT(http_str, 1000, 100);
	A7670E_NETService_Term();

}
