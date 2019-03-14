/*
 * opcua_pubsub_pub.h
 *
 *  Created on: 12.03.2019
 *      Author: miosga.mario
 */

#ifndef SRC_IOT_PUBLISH_OPC_UA_OPCUA_PUBSUB_PUB_H_
#define SRC_IOT_PUBLISH_OPC_UA_OPCUA_PUBSUB_PUB_H_

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

#define PRSB_25

#ifdef PRSB_25
#include "prsb25.h"
#elif defined MB_1222
#include "mb1222.h"
#endif

#define TAG "OPCUA_PUB_SUB"
#define MaxQueueSize 1
#define OPC_UA_BUF_SIZE 500

	uint8_t temprature_sens_read();
	uint8_t mac[6];

	void opcua_pubsub_pub_task(void *pvParameter);
	void addPubSubConnection(UA_Server *server, UA_String connectionName, UA_String addressUrl, UA_NodeId *assignedId);
	void addPublishedDataSet(UA_Server *server, UA_String pdsName, UA_NodeId *assignedId);
	void addWriterGroup(UA_Server *server, UA_NodeId parentConnection, UA_String name, UA_Duration interval, UA_NodeId *assignedId);
	void addDataSetWriter(UA_Server *server, UA_NodeId parentWriterGroup, UA_NodeId connectedPDS, UA_String name, UA_NodeId *assignedId);
	void addNewDataSetField(UA_Server *server, UA_String name, UA_NodeId valueNodeId, UA_NodeId publishedDataSetIdent);
	void removeNode(UA_Server *server, UA_NodeId nodeId);
	void update(UA_Server *server);
	UA_NodeId findNode(UA_Server *server_, UA_QualifiedName targetName, UA_NodeId referenceTypeId, UA_NodeId startingNode);
	void writeMACadr(UA_Server *server);
	void parseTemperature(UA_Server *server, const UA_NodeId nodeid);
	void parseSensorValue(UA_Server *server, const UA_NodeId nodeId);

#endif /* SRC_IOT_PUBLISH_OPC_UA_OPCUA_PUBSUB_PUB_H_ */
