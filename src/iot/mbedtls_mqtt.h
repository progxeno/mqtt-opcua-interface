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
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

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

#include "mb1222.h"
#include "MQTTClient.h"

#define WIFI_SSID "smc@iot"
#define WIFI_PASS "12345678iot"

#define MQTT_SERVER "raspberrypi"
#define MQTT_USER "mbedtls"
#define MQTT_PASS "mbedtlsMQTT"
#define MQTT_PORT 8883
#define MQTT_BUF_SIZE 1000
#define MQTT_WEBSOCKET 0  // 0=no 1=yes

	/* The event group allows multiple bits for each event,
	 but we only care about one event - are we connected
	 to the AP with an IP? */
	const static int CONNECTED_BIT = BIT0;

	static unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
	static unsigned char mqtt_readBuf[MQTT_BUF_SIZE];

	/* FreeRTOS event group to signal when we are connected & ready to make a request */
	static EventGroupHandle_t wifi_event_group;

	static esp_err_t event_handler(void *ctx, system_event_t *event) {
		switch (event->event_id) {
			case SYSTEM_EVENT_STA_START:
				esp_wifi_connect();
				break;
			case SYSTEM_EVENT_STA_GOT_IP:
				xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
				break;
			case SYSTEM_EVENT_STA_DISCONNECTED:
				/* This is a workaround as ESP32 WiFi libs don't currently
				 auto-reassociate. */
				esp_wifi_connect();
				xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
				break;
			default:
				break;
		}
		return ESP_OK;
	}

	void initialise_wifi(void);
	void mqtt_task(void *pvParameters);

#endif /* SRC_IOT_MBEDTLS_MQTT_H_ */
