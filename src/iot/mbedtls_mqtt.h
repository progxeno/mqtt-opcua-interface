/*
 * mbedtls_mqtt.h
 *
 *  Created on: 27.11.2018
 *      Author: Mario
 */

#ifndef SRC_IOT_MBEDTLS_MQTT_H_
#define SRC_IOT_MBEDTLS_MQTT_H_

#include <string.h>
#include <stdlib.h>

#include "sdkconfig.h"

#include "../mbedtls/MQTTClient.h"

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

#include "../driver/mb1222.h"
/* The examples use simple WiFi configuration that you can set via
 'make menuconfig'.

 If you'd rather not, just change the below entries to strings with
 the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
 */
#define WIFI_SSID "smc@iot"
#define WIFI_PASS "12345678iot"

/* Constants that aren't configurable in menuconfig */
#define MQTT_SERVER "raspberrypi"
#define MQTT_USER "device"
#define MQTT_PASS "device@mqtt"
#define MQTT_PORT 8883
#define MQTT_BUF_SIZE 1000
#define MQTT_WEBSOCKET 0  // 0=no 1=yes

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

static void initialise_wifi(void) {
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK (esp_wifi_set_storage(WIFI_STORAGE_RAM) );wifi_config_t
	wifi_config = { .sta = { .ssid = WIFI_SSID, .password = WIFI_PASS, }, };
	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...",
			wifi_config.sta.ssid);
	ESP_ERROR_CHECK (esp_wifi_set_mode(WIFI_MODE_STA) );ESP_ERROR_CHECK
	(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK (esp_wifi_start() );}

static void mqtt_task(void *pvParameters) {
	Network network;
	ESP_ERROR_CHECK(i2c_master_init());

	int ret;
	char buf[10];
	uint8_t sensor_data_h, sensor_data_l;
	uint16_t sensor_data;
	/* Wait for the callback to set the CONNECTED_BIT in the
	 event group.
	 */
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
	false, true, portMAX_DELAY);
	ESP_LOGI(TAG, "Connected to AP");

	ESP_LOGI(TAG, "Start MQTT Task ...");

	MQTTClient client;
	NetworkInit(&network);
	network.websocket = MQTT_WEBSOCKET;

	ESP_LOGI(TAG, "NetworkConnect %s:%d ...", MQTT_SERVER, MQTT_PORT);
	NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
	ESP_LOGI(TAG, "MQTTClientInit  ...");
	MQTTClientInit(&client, &network, 2000,            // command_timeout_ms
			mqtt_sendBuf,         //sendbuf,
			MQTT_BUF_SIZE, //sendbuf_size,
			mqtt_readBuf,         //readbuf,
			MQTT_BUF_SIZE  //readbuf_size
			);

	MQTTString clientId = MQTTString_initializer;
	clientId.cstring = "ESP32MQTT";

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID = clientId;
	data.willFlag = 0;
	data.MQTTVersion = 4; // 3 = 3.1 4 = 3.1.1
	data.keepAliveInterval = 60;
	data.cleansession = 1;

	ESP_LOGI(TAG, "MQTTConnect  ...");
	ret = MQTTConnect(&client, &data);
	if (ret != SUCCESS) {
		ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
	}
	while (1) {

		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data_h,
				&sensor_data_l);
		sensor_data = (uint16_t) sensor_data_h << 8 | sensor_data_l;
		sprintf(buf, "%u", sensor_data);

		MQTTMessage message;
		//	ESP_LOGI(TAG, "MQTTPublish  ... %s",(uint8_t *) buf);

		message.qos = QOS0;
		message.retained = false;
		message.dup = false;
		message.payload = (uint8_t *) buf;
		message.payloadlen = strlen(buf) + 1;

		if (status) {
			if (ret == ESP_ERR_TIMEOUT) {
				ESP_LOGE(TAG, "I2C Timeout");
			} else if (ret == ESP_OK) {
				MQTTPublish(&client, "/test/test>", &message);
			} else {
				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ",
						esp_err_to_name(ret));
			}
		}
	}
}
#endif /* SRC_IOT_MBEDTLS_MQTT_H_ */
