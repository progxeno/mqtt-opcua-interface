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
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include <time.h>
#include <unistd.h>
#include <lwip/sockets.h>
#include "tcpip_adapter.h"
#include "open62541.h"
//#include "myNodeSet.h"

#define TAG "OPC_UA_PUBSUB"
#define BLINK_GPIO 1

	UA_ServerConfig *config;
	uint8_t temprature_sens_read();
	static UA_Boolean running = true;

	void addTemperatureNode(UA_Server *server);
	void sensor_task(void *pvParameter);
	void opcua_task(void *pvParameter);
	void addTemperatureNode(UA_Server *server);
	esp_err_t event_handler(void *ctx, system_event_t *event);
	void wifi_scan2(void);
#endif /* SRC_IOT_OPC_UA_OPCUA_SERVER_H_ */
