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
#include "esp_log.h"

#include "rom/ets_sys.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include <pthread.h>

#include <time.h>
#include <unistd.h>
#include <lwip/sockets.h>
#include "open62541.h"
//#include "global.h"

#define MB_1222

#ifdef PRSB_25
#include "prsb25.h"
#elif defined MB_1222
#include "mb1222.h"
#endif

#define TAG "OPCUA_PUB_SUB"
#define MaxQueueSize 1
#define OPC_UA_BUF_SIZE 500

	uint8_t mac[6];

	uint8_t temprature_sens_read();

	static UA_Boolean running = true;
	static UA_Server *server = NULL;
	UA_NodeId createdNodeId;
	UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent;
	extern xQueueHandle MyQueueHandleId;
	extern SemaphoreHandle_t xSemaphore;
	extern TaskHandle_t TaskMQTT;
	extern TaskHandle_t TaskOPCUA;

	void opcua_pubsub_task(void *pvParameter);
	void addPubSubConnection(UA_Server *server);
	void addPublishedDataSet(UA_Server *server);
	void addDataSetField(UA_Server *server);
	void addWriterGroup(UA_Server *server);
	void addDataSetWriter(UA_Server *server);
	char* deblank(char* input);
	void removeNode(UA_Server *server, UA_NodeId nodeId);
	void parseTemperature(UA_Server *server, const UA_NodeId nodeid);

#endif /* SRC_IOT_OPCUA_PUBSUB_H_ */
