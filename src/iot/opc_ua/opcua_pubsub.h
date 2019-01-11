/*
 * opcua_pubsub.h
 *
 *  Created on: 09.01.2019
 *      Author: miosga.mario
 */

#ifndef SRC_IOT_OPCUA_PUBSUB_H_
#define SRC_IOT_OPCUA_PUBSUB_H_

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

#include "soc/sens_reg.h"

#include <time.h>
#include <unistd.h>
#include <lwip/sockets.h>
#include "open62541.h"
#include "DHT22.h"

#define DEFAULT_SSID "MasterarbeitPi"
#define DEFAULT_PWD "MasterSMC2018"
//#define DEFAULT_SSID "smc@iot"
//#define DEFAULT_PWD "12345678iot"

#define TAG "OPCUA_SERVER"
#define DHT_GPIO 4

uint8_t mac[6];

static UA_Boolean running = true;
UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent;

void addPubSubConnection(UA_Server *server);
void addPublishedDataSet(UA_Server *server);
void addDataSetField(UA_Server *server);
void addWriterGroup(UA_Server *server);
void addDataSetWriter(UA_Server *server);
void parseTemperature(UA_Server *server, const UA_NodeId nodeid);
void opcua_task(void *pvParameter);
static esp_err_t event_handler(void *ctx, system_event_t *event);
void wifi_scan(void);

#endif /* SRC_IOT_OPCUA_PUBSUB_H_ */
