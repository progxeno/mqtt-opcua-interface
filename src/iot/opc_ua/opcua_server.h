/*
 * opcua_server.h
 *
 *  Created on: 11.01.2019
 *      Author: miosga.mario
 */

#ifndef SRC_IOT_OPC_UA_OPCUA_SERVER_H_
#define SRC_IOT_OPC_UA_OPCUA_SERVER_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include <time.h>
#include <unistd.h>
#include <lwip/sockets.h>
#include "tcpip_adapter.h"
#include "open62541.h"

#define TAG "OPC_UA_SERVER"
#define BLINK_GPIO 1

	UA_ServerConfig *config;
	uint8_t temprature_sens_read();
//	static UA_Boolean running = true;
	extern SemaphoreHandle_t xSemaphore;

	void opcua_server_task(void *pvParameter);
	void addTemperatureNode(UA_Server *server);
	void sensor_task(void *pvParameter);
	void addTemperatureNode(UA_Server *server);

#endif /* SRC_IOT_OPC_UA_OPCUA_SERVER_H_ */
