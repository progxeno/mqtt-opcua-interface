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
#include "esp_log.h"

#include "sdkconfig.h"

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

#define TAG "ESP_MQTT"

	uint8_t temprature_sens_read();
	uint8_t mac[6];

	extern const uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
	extern const uint8_t ca_pem_end[] asm("_binary_ca_pem_end");

	void mqtt_esp_task(void *pvParameters);
	int sendOnlineMsg(esp_mqtt_client_handle_t client, char *macAdr);
	esp_mqtt_client_config_t configureClient(char *macAdr);

#endif /* SRC_IOT_ESP_MQTT_TLS_H_ */
