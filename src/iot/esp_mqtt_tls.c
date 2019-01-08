/*
 * esp_mqtt_tls.c
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */

#include "esp_mqtt_tls.h"

void wifi_init(void) {
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	wifi_config_t wifi_config = {
			.sta = {
					.ssid = CONFIG_WIFI_SSID,
					.password =
					CONFIG_WIFI_PASSWORD, }, };
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]", CONFIG_WIFI_SSID, "******");
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "Waiting for wifi");
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

void mqtt_app_start(void) {
	const esp_mqtt_client_config_t mqtt_cfg = {
			.host = "raspberrypi",
			.port = 8883,
			.event_handle = mqtt_event_handler,
			.transport = MQTT_TRANSPORT_OVER_SSL,
			.cert_pem = (const char *) ca_pem_start,
			.username = MQTT_USER,
			.password = MQTT_PASS,
			.client_id = "ESP_MQTT", };

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
}
