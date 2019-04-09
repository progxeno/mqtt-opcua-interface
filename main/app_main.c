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

#define OPCUA_PUBSUB_TO_MQTT

#ifdef ESP_MQTT_TLS
#include "esp_mqtt_tls.h"
#elif defined MBED_TLS_MQTT
#include "mbedtls_mqtt.h"
#elif defined OPCUA_PUB_SUB
#include "opcua_pubsub.h"
#elif defined OPCUA_SERVER
#include "opcua_server.h"
#elif defined OPCUA_PUB_SUB_PUB
#include "opcua_pubsub_pub.h"
#elif defined MQTT_TO_OPCUA_PUBSUB
#include "mbedtls_sub_mqtt.h"
#include "opcua_pub.h"
#elif defined OPCUA_PUBSUB_TO_MQTT
#include "opcua_sub.h"
#include "mbedtls_pub_mqtt.h"
#endif

#define MaxQueueSize 3
#define MaxElementsPerQueue 500
xQueueHandle MyQueueHandleId;

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
const static int CONNECTED_BIT = BIT0;
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* Taskhandler for two parallel Tasks */
TaskHandle_t TaskMQTT;
TaskHandle_t TaskOPCUA;
SemaphoreHandle_t xSemaphore = NULL;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	xSemaphore = xSemaphoreCreateMutex();

	MyQueueHandleId = xQueueCreate(MaxQueueSize, MaxElementsPerQueue);
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			/* Create the semaphore to guard a shared resource.  As we are using
			 the semaphore for mutual exclusion we create a mutex semaphore
			 rather than a binary semaphore. */
#ifdef ESP_MQTT_TLS
			xTaskCreatePinnedToCore(mqtt_esp_task, "mqtt_esp_task", 16384, NULL, tskIDLE_PRIORITY + 1, &TaskMQTT, 1);
#elif defined MBED_TLS_MQTT
			xTaskCreatePinnedToCore(mqtt_mbedtls_task, "mqtt_mbedtls_task", 20000, NULL, tskIDLE_PRIORITY + 1, &TaskMQTT, 1);
#elif defined OPCUA_PUB_SUB
//			xTaskCreate(&opcua_pubsub_task, "opcua_pubsub_task", 32768, NULL, 1, NULL);
#elif defined OPCUA_SERVER
			xTaskCreate(&opcua_server_task, "opcua_server_task", 32768, NULL, 1, NULL);
#elif defined MQTT_TO_OPCUA_PUBSUB
			vTaskDelay(1000 / portTICK_RATE_MS);
			xTaskCreatePinnedToCore(opcua_pubsub_task, "opcua_pubsub_task", 16384, NULL, tskIDLE_PRIORITY + 1, &TaskOPCUA, 1);
			vTaskDelay(500 / portTICK_RATE_MS);
			xTaskCreatePinnedToCore(mqtt_mbedtls_sub_task, "mqtt_mbedtls_sub_task", 10240, NULL, tskIDLE_PRIORITY + 2, &TaskMQTT, 1);
#elif defined OPCUA_PUB_SUB_PUB
			xTaskCreatePinnedToCore(opcua_pubsub_pub_task, "opcua_pubsub_pub_task", 16384, NULL, tskIDLE_PRIORITY + 1, &TaskOPCUA, 0);
#elif defined OPCUA_PUBSUB_TO_MQTT
			vTaskDelay(1000 / portTICK_RATE_MS);
			xTaskCreatePinnedToCore(opcua_sub_task, "opcua_sub_task", 16384, NULL, tskIDLE_PRIORITY + 1, &TaskOPCUA, 1);
			vTaskDelay(500 / portTICK_RATE_MS);
			xTaskCreatePinnedToCore(mqtt_mbedtls_pub_task, "mqtt_mbedtls_pub_task", 12800, NULL, tskIDLE_PRIORITY + 1, &TaskMQTT, 1);

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
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()

	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	esp_wifi_set_ps(WIFI_PS_NONE);
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
	vTaskDelay(500 / portTICK_RATE_MS);
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
