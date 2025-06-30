/*
 * A7670.c
 *
 *  Created on: Feb 12, 2025
 *      Author: Stanislaus Albert Adiwijaya
 */

#include "A7670.h"

void A7670E_sendAT(char cmd[], uint32_t tx_timeout, uint32_t rx_timeout)
{
//	char ATcommand[150];
	uint8_t rx_buffer[150] = {0};

	memset(rx_buffer,0,sizeof(rx_buffer));

//	sprintf(ATcommand, cmd);
//	HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), tx_timeout);
	HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), tx_timeout);
	HAL_UART_Receive(&huart1, rx_buffer, 100, rx_timeout);
	HAL_Delay(1000);
	UART_SEND(&huart2, rx_buffer);
//	memset(rx_buffer, 0, sizeof(rx_buffer));
}

void A7670E_sendSMS(char mobileNumber[], char msg_content[])
{
//	char mobileNumber[] = "+491745749140";  // Enter the Mobile Number you want to send to
	char ATcommand[80];
	uint8_t buffer[30] = {0};
	uint8_t ATisOK = 0;
	while(!ATisOK){
		sprintf(ATcommand,"AT\r\n");
		HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
		HAL_UART_Receive(&huart1, buffer, 30, 100);
		HAL_Delay(1000);
		UART_SEND(&huart2, (char *)buffer);
		if(strstr((char *)buffer,"OK")){
			ATisOK = 1;
		}
		HAL_Delay(1000);
		memset(buffer,0,sizeof(buffer));
	}
	sprintf(ATcommand,"AT+CMGF=1\r\n");
	HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	HAL_UART_Receive(&huart1, buffer, 30, 100);
	HAL_Delay(1000);
	UART_SEND(&huart2, (char *)buffer);
	memset(buffer,0,sizeof(buffer));
	sprintf(ATcommand,"AT+CMGS=\"%s\"\r\n",mobileNumber);
	HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	HAL_UART_Receive(&huart1, buffer, 30, 100);
	HAL_Delay(1000);
	UART_SEND(&huart2, (char *)buffer);
//	sprintf(ATcommand,"https://maps.google.com/?q=51.5622406N,8.1138048E %c \r\n", 0x1A);
	HAL_UART_Transmit(&huart1,(uint8_t *)msg_content,strlen(msg_content),1000);
	HAL_UART_Receive(&huart1, buffer, 30, 100);
	UART_SEND(&huart2, (char *)buffer);
	sprintf(ATcommand,"%c \r\n", 0x1A);
	HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),1000);
	HAL_UART_Receive(&huart1, buffer, 30, 100);
	UART_SEND(&huart2, (char *)buffer);
	memset(buffer,0,sizeof(buffer));
	HAL_Delay(4000);

}

void A7670E_NETService_Init()
{
	A7670E_sendAT("AT\r\n", 3000, 100);
	A7670E_sendAT("AT+CICCID\r\n", 3000, 100);
	A7670E_sendAT("AT+SIMCOMATI\r\n", 3000, 100);
	A7670E_sendAT("AT+COPS?\r\n", 3000, 100);
	A7670E_sendAT("AT+CPIN?\r\n", 3000, 100);
//	A7670E_sendAT("AT+CNMP=2\r\n", 3000, 100);
	A7670E_sendAT("AT+CSQ\r\n", 3000, 100);
	A7670E_sendAT("AT+CREG?\r\n", 3000, 100);
	A7670E_sendAT("AT+CGREG?\r\n", 3000, 100);
	A7670E_sendAT("AT+CPSI?\r\n", 3000, 100);

	A7670E_sendAT("AT+NETCLOSE\r\n", 3000, 100);
	A7670E_sendAT("AT+CGDCONT=1,\"IP\",\"internet\"\r\n", 3000, 100);
	A7670E_sendAT("AT+CGATT=1\r\n", 1000, 100);

	A7670E_sendAT("AT+CIPMODE=0\r\n", 1000, 100);
	A7670E_sendAT("AT+CIPSENDMODE=0\r\n", 2000, 100);
	A7670E_sendAT("AT+CIPCCFG=10,0,0,0,1,0,75000\r\n", 2000, 100);
	A7670E_sendAT("AT+CIPTIMEOUT=75000,15000,15000\r\n", 1000, 100);
	A7670E_sendAT("AT+NETOPEN\r\n", 1000, 100);
	A7670E_sendAT("AT+NETOPEN?\r\n", 1000, 100);

	A7670E_sendAT("AT+IPADDR\r\n", 1000, 100);

}

void A7670E_NETService_Term()
{
	A7670E_sendAT("AT+CIPCLOSE=0\r\n", 1000, 100);
	A7670E_sendAT("AT++NETCLOSE\r\n", 1000, 100);

}

NTP_datetime_t A7670E_getDateTime()
{
	char ATcommand[80] = "AT+CCLK?\r\n";
	uint8_t rx_buffer[20] = {0};

	HAL_UART_Transmit(&huart1, (uint8_t *)ATcommand, strlen(ATcommand), 1000);
	HAL_UART_Receive(&huart1, rx_buffer, 100, 100);
//	HAL_Delay(1000);
	UART_SEND(&huart2, (char *)rx_buffer);

	// Parse the response to extract date and time
	char *start = strchr((char *)rx_buffer, '\"') + 1; // Find the first quote
	char *end = strchr(start, '\"'); // Find the second quote

	if (end != NULL) {
		*end = '\0'; // Null terminate to isolate the string
	}

	// Split the date and time
	char *date_time = strtok(start, ",");
	char *time_str = strtok(NULL, "+");

	NTP_datetime_t NTP_data;

	if (date_time != NULL && time_str != NULL) {
		// Parse date and time
		sscanf(date_time, "%u/%u/%u", &NTP_data.year, &NTP_data.month, &NTP_data.day);
		sscanf(time_str, "%u:%u:%u", &NTP_data.hours, &NTP_data.minutes, &NTP_data.seconds);

	}

	return NTP_data;

}


void A7670E_SyncDateTime()
{
	A7670E_sendAT("AT+CTZR=?\r\n", 1000, 100);
//	A7670E_sendAT("AT+CNTP=\"pool.ntp.org\",8\r\n", 1000, 100);
	A7670E_sendAT("AT+CNTP=\"pool.ntp.org\",8\r\n", 1000, 100);
	A7670E_sendAT("AT+CNTP\r\n", 1000, 100);
}

/*
const char apn[]  = "internet"; // Change this to your Provider details
const char host[] = "tcp://test.mosquitto.org"; // Change this to your host
const int  port = 1883;
const char username[] = "";
const char password[] = "";
const char topic[] = "pot/adc/1";
const uint32_t timeOut =10000;
char ATcommand[80];
uint8_t buffer[160] = {0};
uint8_t ATisOK = 0;
uint8_t CGREGisOK = 0;
uint32_t previousTick;
uint16_t readValue;
char charData[15];

void SIMTransmit(char *cmd)
{
  memset(buffer,0,sizeof(buffer));
  HAL_UART_Transmit(&huart1,(uint8_t *)cmd,strlen(cmd),1000);
  HAL_UART_Receive (&huart1, buffer, 100, 1000);
}

void mqttPublish(void)
{
  ATisOK = 0;
  CGREGisOK = 0;

  // Check for OK response for AT
  previousTick =  HAL_GetTick();
  while(!ATisOK && previousTick  + timeOut >  HAL_GetTick())
  {
    SIMTransmit("AT\r\n");
    HAL_Delay(1000);
    if(strstr((char *)buffer,"OK"))
    {
      ATisOK = 1;
    }
  }

  // Check for network registration.
  if(ATisOK)
  {
    previousTick =  HAL_GetTick();
    while(!CGREGisOK  && previousTick  + timeOut >  HAL_GetTick())
    {
      SIMTransmit("AT+CGREG?\r\n");
      if(strstr((char *)buffer,"+CGREG: 0,1")) // Use 0,5 For Roaming
      {
        CGREGisOK = 1;
      }
    }
  }

  // If registered
  if(CGREGisOK)
  {
	sprintf(ATcommand,"AT+CGSOCKCONT=1,\"IP\",\"%s\"\r\n",apn); // Specify the value of  PDP context
	SIMTransmit(ATcommand);
	SIMTransmit("AT+CMQTTSTART\r\n"); // Start MQTT Service
	SIMTransmit("AT+CMQTTACCQ=0,\"client01\"\r\n"); // Acquire a Client
	sprintf(ATcommand,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1\r\n",host,port); // Connect to a MQTT Server
	// sprintf(ATcommand,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1,%s,%s\r\n",host,port,username,password);
	SIMTransmit(ATcommand);
	sprintf(ATcommand,"AT+CMQTTTOPIC=0,%d\r\n",strlen(topic)); // Set the topic for publish message
	SIMTransmit(ATcommand);
	sprintf(ATcommand,"%s\r\n",topic);
	SIMTransmit(ATcommand);
	sprintf(ATcommand,"AT+CMQTTPAYLOAD=0,%d\r\n",strlen(charData)-2); // Set the payload
	SIMTransmit(ATcommand);
	SIMTransmit(charData);
	SIMTransmit("AT+CMQTTPUB=0,1,60\r\n"); // Publish
	SIMTransmit("AT+CMQTTDISC=0,120\r\n"); // Disconnect from Server
	SIMTransmit("AT+CMQTTREL=0\r\n"); // Release the Client
	SIMTransmit("AT+CMQTTSTOP\r\n"); // Stop MQTT Service
  }
}
*/
