///*
// * opcua_sub.h
// *
// *  Created on: 13.03.2019
// *      Author: miosga.mario
// */
//
//#ifndef SRC_IOT_BRIDGE_OPCUA_SUB_H_
//#define SRC_IOT_BRIDGE_OPCUA_SUB_H_
//#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
//extern "C" {
//#endif
//
//	/* Files myNS.h and myNS.c are created from myNS.xml */
//#include "myNS.h"
//
//#include <stdio.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "driver/gpio.h"
//#include "sdkconfig.h"
//
//#include "freertos/FreeRTOS.h"
//#include "freertos/event_groups.h"
//#include "esp_log.h"
//
//#include "rom/ets_sys.h"
//#include "soc/rtc_cntl_reg.h"
//#include "soc/sens_reg.h"
//
//#include <pthread.h>
//
//#include <time.h>
//#include <unistd.h>
//#include <lwip/sockets.h>
//#include "open62541.h"
//#include "cJSON.h"
//
//#define TAG "OPCUA_PUB_SUB"
//#define MaxQueueSize 1
//#define OPC_UA_BUF_SIZE 500
//
//	uint8_t mac[6];
//
//	extern xQueueHandle MyQueueHandleId;
//	extern SemaphoreHandle_t xSemaphore;
//
//	void opcua_sub_task(void *pvParameter);
//	void subscriptionPollingCallback(UA_Server *server, UA_PubSubConnection *connection);
//
//#endif /* SRC_IOT_BRIDGE_OPCUA_SUB_H_ */
