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

#include "cJSON.h"
#include "mqtt_client.h"

#define PRSB_25

#ifdef PRSB_25
#include "prsb25.h"
#elif defined MB_1222
#include "mb1222.h"
#endif

	/* The event group allows multiple bits for each event,
	 but we only care about one event - are we connected
	 to the AP with an IP? */
	uint8_t temprature_sens_read();
	uint8_t mac[6];
	const static int CONNECTED_BIT = BIT0;
	static EventGroupHandle_t wifi_event_group;

	extern const uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
	extern const uint8_t ca_pem_end[] asm("_binary_ca_pem_end");

	static esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
	static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

	void wifi_init(void);
	void mqtt_app_start(void);

#endif /* SRC_IOT_ESP_MQTT_TLS_H_ */
