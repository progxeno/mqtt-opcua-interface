/*
 * mbedtls_pub_mqtt.h
 *
 *  Created on: 09.04.2019
 *      Author: miosga.mario
 */

#ifndef SRC_IOT_BRIDGE_MBEDTLS_PUB_MQTT_H_
#define SRC_IOT_BRIDGE_MBEDTLS_PUB_MQTT_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "cJSON.h"
#include "MQTTClient.h"
//#include "global.h"

#define TAG "MBEDTLS_MQTT"
#define MaxQueueSize 3
#define MQTT_BUF_SIZE 500
#define MQTT_WEBSOCKET 0  // 0=no 1=yes

	uint8_t temprature_sens_read();

	uint8_t mac[6];

	unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
	unsigned char mqtt_readBuf[MQTT_BUF_SIZE];
	extern xQueueHandle MyQueueHandleId;
	extern SemaphoreHandle_t xSemaphore;
	extern TaskHandle_t TaskMQTT;
	extern TaskHandle_t TaskOPCUA;
	void startClient(MQTTClient *client, Network *network, MQTTPacket_connectData *data);
	void setMac(char *macAdr);
	void mqtt_mbedtls_pub_task(void *pvParameters);
	void sendOnlineMsg(MQTTClient client, char *macAdr);
	void configureClient(MQTTPacket_connectData *data, char *macAdr);

#endif /* SRC_IOT_BRIDGE_MBEDTLS_PUB_MQTT_H_ */
