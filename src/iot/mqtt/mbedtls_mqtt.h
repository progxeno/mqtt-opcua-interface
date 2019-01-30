/*
 * mbedtls_mqtt.h
 *
 *  Created on: 27.11.2018
 *      Author: Mario
 */

#ifndef SRC_IOT_MBEDTLS_MQTT_H_
#define SRC_IOT_MBEDTLS_MQTT_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

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

#define MB_1222

#ifdef PRSB_25
#include "prsb25.h"
#elif defined MB_1222
#include "mb1222.h"
#endif

#define TAG "MBEDTLS_MQTT"
#define MQTT_BUF_SIZE 1000
#define MQTT_WEBSOCKET 0  // 0=no 1=yes

	uint8_t temprature_sens_read();

	uint8_t mac[6];

	unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
	unsigned char mqtt_readBuf[MQTT_BUF_SIZE];

	void mqtt_mbedtls_task(void *pvParameters);

#endif /* SRC_IOT_MBEDTLS_MQTT_H_ */
