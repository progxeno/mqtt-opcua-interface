/*
 * esp_mqtt_tls.h
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */

#ifndef SRC_IOT_ESP_MQTT_TLS_H_
#define SRC_IOT_ESP_MQTT_TLS_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "mb1222.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define MQTT_USER "espMQTT"
#define MQTT_PASS "esp"

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
const static int CONNECTED_BIT = BIT0;
static EventGroupHandle_t wifi_event_group;

extern const uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
extern const uint8_t ca_pem_end[] asm("_binary_ca_pem_end");

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		break;
	default:
		break;
	}
	return ESP_OK;
}
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {

	ESP_ERROR_CHECK(i2c_master_init());
	esp_mqtt_client_handle_t client = event->client;

	// your_context_t *context = event->context;
	int ret;
	char buf[10];
	uint8_t sensor_data_h, sensor_data_l;
	uint16_t sensor_data;

	while (1) {
		//fputs(status ? "true" : "false", stdout);

		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data_h,
				&sensor_data_l);
		sensor_data = (uint16_t) sensor_data_h << 8 | sensor_data_l;
		sprintf(buf, "%u", sensor_data);

		if (status) {
			if (ret == ESP_ERR_TIMEOUT) {
				ESP_LOGE(TAG, "I2C Timeout");
			} else if (ret == ESP_OK) {
				esp_mqtt_client_publish(client, "device/id1/data", buf, 0, 0,
						0);
			} else {
				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ",
						esp_err_to_name(ret));
			}
		}
	}

	return ESP_OK;
}

void wifi_init(void);
void mqtt_app_start(void);

#endif /* SRC_IOT_ESP_MQTT_TLS_H_ */
