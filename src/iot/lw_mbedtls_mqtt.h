/*
 * lw_mbedtls_mqtt.h
 *
 *  Created on: 29.11.2018
 *      Author: Mario
 */

#ifndef SRC_IOT_LW_MBEDTLS_MQTT_H_
#define SRC_IOT_LW_MBEDTLS_MQTT_H_


#include <stdlib.h>
#include <esp_mqtt.h>

#include <lwmqtt/unix.h>

//#include "cJSON.h"
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

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
//const static int CONNECTED_BIT = BIT0;

/* Constants that aren't configurable in menuconfig */
#define MQTT_SERVER "raspberrypi"
#define MQTT_USER "device"
#define MQTT_PASS "device@mqtt"
#define MQTT_PORT 8883
#define MQTT_BUF_SIZE 1000
#define MQTT_WEBSOCKET 0  // 0=no 1=yes

#define COMMAND_TIMEOUT 5000

static unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
static unsigned char mqtt_readBuf[MQTT_BUF_SIZE];

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler2(void *ctx, system_event_t *event) {
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

static void initialise_wifi2(void) {
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler2, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK (esp_wifi_set_storage(WIFI_STORAGE_RAM) );wifi_config_t
	wifi_config = { .sta = { .ssid = WIFI_SSID, .password = WIFI_PASS, }, };
	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...",
			wifi_config.sta.ssid);
	ESP_ERROR_CHECK (esp_wifi_set_mode(WIFI_MODE_STA) );ESP_ERROR_CHECK
	(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK (esp_wifi_start() );}

static void mqtt_task2(void *pvParameters) {
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

	lwmqtt_client_t client;
	NetworkInit(&network);
	network.websocket = MQTT_WEBSOCKET;

	ESP_LOGI(TAG, "NetworkConnect %s:%d ...", MQTT_SERVER, MQTT_PORT);
	NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
	ESP_LOGI(TAG, "MQTTClientInit  ...");

	lwmqtt_init(&client,
			mqtt_sendBuf,         //sendbuf,
			MQTT_BUF_SIZE, //sendbuf_size,
			mqtt_readBuf,         //readbuf,
			MQTT_BUF_SIZE  //readbuf_size
			);

	lwmqtt_options_t options = lwmqtt_default_options;
	  options.client_id = lwmqtt_string("lwmqtt");
	  options.keep_alive = 60;

	  lwmqtt_return_code_t return_code;
	    lwmqtt_connect(&client, options, NULL, &return_code, COMMAND_TIMEOUT);

	while (1) {

		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data_h,
				&sensor_data_l);
		sensor_data = (uint16_t) sensor_data_h << 8 | sensor_data_l;
		sprintf(buf, "%u", sensor_data);

		 // prepare message
		 lwmqtt_message_t msg = {
				 .qos = LWMQTT_QOS0,
				 .retained = false,
				 .payload = (uint8_t *) buf,
				 .payload_len = strlen(buf) + 1
		 };

		if (status) {
			if (ret == ESP_ERR_TIMEOUT) {
				ESP_LOGE(TAG, "I2C Timeout");
			} else if (ret == ESP_OK) {
				ESP_LOGI(TAG, "lwmqtt");
				lwmqtt_publish(&client, lwmqtt_string("/test/lwmqtt"), msg, COMMAND_TIMEOUT);
			} else {
				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ",
						esp_err_to_name(ret));
			}
		}
	}
}

#endif /* SRC_IOT_LW_MBEDTLS_MQTT_H_ */
