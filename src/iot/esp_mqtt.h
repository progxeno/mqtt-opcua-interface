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

#include "../driver/mb1222.h"

#include "esp_log.h"

static EventGroupHandle_t wifi_event_group;
//const static int CONNECTED_BIT = BIT0;

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

static void wifi_init(void) {
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK (esp_wifi_set_storage(WIFI_STORAGE_RAM));wifi_config_t
	wifi_config = { .sta = { .ssid = CONFIG_WIFI_SSID, .password =
	CONFIG_WIFI_PASSWORD, }, };
	ESP_ERROR_CHECK (esp_wifi_set_mode(WIFI_MODE_STA));ESP_ERROR_CHECK
	(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]", CONFIG_WIFI_SSID,
			"******");
	ESP_ERROR_CHECK (esp_wifi_start());ESP_LOGI
	(TAG, "Waiting for wifi");
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true,
			portMAX_DELAY);
}

extern const uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
extern const uint8_t ca_pem_end[] asm("_binary_ca_pem_end");

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
				esp_mqtt_client_publish(client, "/device/id1/data/range", buf,
						0, 0, 0);
			} else {
				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ",
						esp_err_to_name(ret));
			}
		}
	}

	return ESP_OK;
}

static void mqtt_app_start(void) {
	const esp_mqtt_client_config_t mqtt_cfg = { .host = "raspberrypi", .port =
			8883, .event_handle = mqtt_event_handler, .transport =
			MQTT_TRANSPORT_OVER_SSL, .cert_pem = (const char *) ca_pem_start, };

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
}
