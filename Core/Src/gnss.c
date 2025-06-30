/*
 * gnss.c
 *
 *  Created on: Feb 12, 2025
 *      Author: Stanislaus Albert Adiwijaya
 */

#include "gnss.h"

void GNSS_Init(uint32_t startup_time)
{
	A7670E_sendAT("AT+CGNSSPWR=1\r\n", 100, 100);
	HAL_Delay(30000);
//	A7670E_sendAT("AT+CGPSCOLD=?\r\n", 1000, 100);
//	A7670E_sendAT("AT+CGPSCOLD\r\n", 1000, 100);
	A7670E_sendAT("AT+CGNSSTST=1\r\n", 1000, 100);
//	A7670E_sendAT("AT+CGNSSMODE?\r\n", 1000, 100);
	A7670E_sendAT("AT+CGNSSPORTSWITCH=0,1\r\n", 10000, 100);
//	HAL_Delay(10000);
	A7670E_sendAT("\r\nAT+CGNSSINFO\r\n", 10000, 100);
	HAL_Delay(5000);
	A7670E_sendAT("AT\r\n", 1000, 100);
//	A7670E_sendAT("AT+CGNSSPWR?\r\n", 100, 100);
	A7670E_sendAT("AT+CGNSSPWR=1\r\n", 100, 100);
	HAL_Delay(startup_time);
	A7670E_sendAT("AT+CGNSSTST=1\r\n", 1000, 100);
//	HAL_Delay(1000);
//	A7670E_sendAT("AT+CGNSSMODE?\r\n", 1000, 100);
//	  A7670E_sendAT("AT+CGNSSPORTSWITCH=0,1\r\n", 10000, 10000);
	A7670E_sendAT("AT+CGNSSPORTSWITCH=0,1\r\n", 10000, 100);
}

void GNSS_PowerDown()
{
	A7670E_sendAT("\r\nAT+CGNSSPWR=0\r\n", 100, 100);

}

void getPos(uint8_t rep)
{
	GNSS_Init(10000);

	int i;
	for(i=0; i<rep; i++)
	{
		UART_SEND(&huart2, "\r\n");
		A7670E_sendAT("\r\nAT+CGNSSINFO\r\n", 10000, 100);
	}

	UART_SEND(&huart2, "\r\n");
	HAL_Delay(1000);
	GNSS_PowerDown();
	HAL_Delay(1000);

}

/*
 *****GNSS OUTPUT DATA FORMAT*****
 *
 * +CGNSSINFO:
 * [<mode>],[<GPS-SVs>],[BEIDOU-SVs],[<GLONASS-SVs>],[<GALILEO-SVs>],[<lat>],[<N/S>],
 * [<log>],[<E/W>],[<date>],[<UTC-time>],[<alt>],[<speed> ],[<course>],[<PDOP>]
 *
 * Return value [Latitude, Longitude]
 *
 */
char *getCharPos(char *rx_buffer, uint32_t buffer_size)
{
	char ATcommand[80] = "AT+CGNSSINFO\r\n";
	char buffer[86] = {0};
//	char resp_buffer[25] = {0};

//	float lat, lon;
	char lat[12] = {0};
	char lon[12] = {0};
	char latDir[2] = {0};
	char lonDir[2] = {0};
	char latSign[2] = {0};
	char lonSign[2] = {0};

	GNSS_Init(10000);

	HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 10000);
	HAL_UART_Receive(&huart1, buffer, 100, 10000);
//	UART_SEND(&huart2, buffer);

//	sprintf(buffer, "+CGNSSINFO:2,10,8,,1,51.4807,N,0.0062,W,23/04/2023,12:00:00,15.2,0.0,0.0,1.5\r\n");
//	sprintf(buffer, "+CGNSSINFO: 3,16,09,,02,51.5622864,N,8.1140766,W,130225,134138.00,168.7,0.171,,2.95\r\n");

	// char ATcommand[80] = "\r\nAT+CGNSSINFO\r\n";
	// uint8_t rx_buffer[150] = {0};
	// HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 10000);
	// HAL_UART_Receive(&huart1, rx_buffer, 100, 10000);
	// HAL_Delay(1000);
	// memcpy(gnssoutput, rx_buffer + 12, sizeof(gnssoutput));
	// sscanf(rx_buffer, "+CGNSSINFO: %d,%d,,%d,%d", &lat, &ns, &lon, &ew);

	// sscanf(rx_buffer, "%*c %*d,%*d,,%*d,%*d,%f,%c,%f,%c,%d,%d", lat, &ns, lon, &ew, date, time);
	// UART_SEND(&huart2, rx_buffer);
	// str_tokenizer(rx_buffer);
	// gnss_data gnss_buffer = gnss_tokenizer(rx_buffer);

//	gnss_t gnss_val;

	const char s[2] = ",";
	char *token;
//	char *ePtr;

	token = strtok(buffer, s);
	token = strtok(NULL, s);
	token = strtok(NULL, s);
//	gnss_val.gpsNumber = strtol(token, &ePtr, 10);
	token = strtok(NULL, s);
//	gnss_val.glonassNumber = strtol(token, &ePtr, 10);
	token = strtok(NULL, s);
//	gnss_val.galileoNumber = strtol(token, &ePtr, 10);
//	token = strtok(NULL, s);
//	lat = strtof(token, &ePtr);
	strcpy(lat, token);
	token = strtok(NULL, s);
	strcpy(latDir, token);
	token = strtok(NULL, s);
//	lon = strtof(token, &ePtr);
	strcpy(lon, token);
	token = strtok(NULL, s);
	strcpy(lonDir, token);
//	token = strtok(NULL, s);
//	strcpy(gnss_val.date, token);
//	token = strtok(NULL, s);
//	strcpy(gnss_val.time, token);
//	token = strtok(NULL, s);
//	gnss_val.alt = strtof(token, &ePtr);
//	token = strtok(NULL, s);
//	gnss_val.speed = strtof(token, &ePtr);
//	token = strtok(NULL, s);
//	gnss_val.pdop = strtof(token, &ePtr);
//	printf("Latitude: %.7f %.7f\n", lat, lon);
//	sprintf(rx_buffer,"%s%0.7f,%0.7f \r\n", lat, lon);

	if(strcmp(latDir, "S") == 0)
	{
		sprintf(latSign,"-");
	}
	if(strcmp(lonDir, "W") == 0)
	{
		sprintf(lonSign,"-");
	}

	snprintf(rx_buffer, buffer_size, "%s%s,%s%s \r\n", latSign, lat, lonSign, lon);
//	UART_SEND(&huart2, "\r\n");
//	UART_SEND(&huart2, latSign);
//	UART_SEND(&huart2, "\r\n");
//	UART_SEND(&huart2, lonSign);
	HAL_Delay(1000);
	GNSS_PowerDown();

    return rx_buffer;
}

coord_t getPosition()
{
	coord_t coordinates;

	char ATcommand[80] = "AT+CGNSSINFO\r\n";
	char buffer[86] = {0};
//	char resp_buffer[25] = {0};

//	float lat, lon;
//	char lat[12] = {0};
//	char lon[12] = {0};
	char latDir[2] = {0};
	char lonDir[2] = {0};
//	char latSign[2] = {0};
//	char lonSign[2] = {0};

//	GNSS_Init(10000);
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
//	gnss_val.gpsNumber = strtol(token, &ePtr, 10);
	token = strtok(NULL, s);
//	gnss_val.glonassNumber = strtol(token, &ePtr, 10);
	token = strtok(NULL, s);
//	gnss_val.galileoNumber = strtol(token, &ePtr, 10);
//	token = strtok(NULL, s);
	coordinates.lat = strtof(token, &ePtr);
//	strcpy(lat, token);
	token = strtok(NULL, s);
	strcpy(latDir, token);
	token = strtok(NULL, s);
	coordinates.lon = strtof(token, &ePtr);
//	strcpy(lon, token);
	token = strtok(NULL, s);
	strcpy(lonDir, token);

	if(strcmp(latDir, "S") == 0)
	{
		coordinates.lat *= -1;
	}

	if(strcmp(lonDir, "W") == 0)
	{
		coordinates.lon *= -1;
	}

	return coordinates;
}

//void GNSS_GetData()
//{
//	char ATcommand[80] = "AT+CGNSSINFO\r\n";
//	uint8_t rx_buffer[120] = {0};
////	char gnssoutput[25];
////	char lat[20], ns, lon[20], ew, date[10], time[15];
////	uint8_t lat, lon;
////	uint8_t ns, ew;
//
////	sprintf(ATcommand, cmd);
////	memset(rx_buffer, 0, sizeof(rx_buffer));
//	HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 10000);
//	HAL_UART_Receive(&huart1, rx_buffer, 100, 10000);
//	HAL_Delay(1000);
////	memcpy(gnssoutput, rx_buffer + 12, sizeof(gnssoutput));
////	sscanf(rx_buffer, "+CGNSSINFO: %d,%d,,%d,%d", &lat, &ns, &lon, &ew);
//
////	sscanf(rx_buffer, "%*c %*d,%*d,,%*d,%*d,%f,%c,%f,%c,%d,%d", lat, &ns, lon, &ew, date, time);
//	UART_SEND(&huart2, rx_buffer);
//}
