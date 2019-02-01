#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "esp_log.h"
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

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
const static int CONNECTED_BIT = BIT0;
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
#ifdef SRC_IOT_ESP_MQTT_TLS_H_
			xTaskCreate(&mqtt_esp_task, "mqtt_esp_task", 32768, NULL, 1, NULL);
#elif defined SRC_IOT_MBEDTLS_MQTT_H_
			xTaskCreate(&mqtt_mbedtls_task, "mqtt_mbedtls_task", 32768, NULL, 1, NULL);
#elif defined SRC_IOT_OPCUA_PUBSUB_H_
			xTaskCreate(&opcua_pubsub_task, "opcua_pubsub_task", 32768, NULL, 1, NULL);
#elif defined SRC_IOT_OPC_UA_OPCUA_SERVER_H_
			xTaskCreate(&opcua_server_task, "opcua_server_task", 32768, NULL, 1, NULL);
#endif
			xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			/* This is a workaround as ESP32 WiFi libs don't currently
			 auto-reassociate. */
			ESP_ERROR_CHECK(esp_wifi_connect())
			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
			break;
		default:
			break;
	}
	return ESP_OK;
}

static void wifi_init(void)
{
	printf("TestespMqtt-Wifi");
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()

	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	wifi_config_t wifi_config = {
			.sta = {
					.ssid = CONFIG_DEFAULT_SSID,
					.password = CONFIG_DEFAULT_PWD, }, };
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]", CONFIG_DEFAULT_SSID, "******");
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "Waiting for wifi");
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

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
	wifi_init();
}
