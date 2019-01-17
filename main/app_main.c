#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

#define ESP_MQTT_TLS

#ifdef ESP_MQTT_TLS
	#include "esp_mqtt_tls.h"
#elif defined MBED_TLS_MQTT
	#include "mbedtls_mqtt.h"
#elif defined OPCUA_PUB_SUB
	#include "opcua_pubsub.h"
#elif defined OPCUA_SERVER
	#include "opcua_server.h"
//#elif defined LWMQTT
//	#include "lw_mbedtls_mqtt.h"
#endif

void app_main()
{
	ESP_LOGI(TAG, "[APP] Startup..");
	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
	esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
	esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

	nvs_flash_init();

#ifdef SRC_IOT_ESP_MQTT_TLS_H_
	wifi_init();
	mqtt_app_start();
//	xTaskCreate(spi_process_task, "spi", 16384, NULL, 5, NULL);
#endif

#ifdef SRC_IOT_MBEDTLS_MQTT_H_
	initialise_wifi();
	//xTaskCreate(&mqtt_task, "mqtt_task", 16384, NULL, 5, NULL);
#endif

#ifdef SRC_IOT_OPCUA_PUBSUB_H_
	wifi_scan();
#endif

#ifdef SRC_IOT_OPC_UA_OPCUA_SERVER_H_
	wifi_scan();
#endif

//		lw_initialise_wifi();
//
//		xTaskCreate(&lw_mqtt_task, "lw_mqtt_task", 16384, NULL, 5, NULL);
}
