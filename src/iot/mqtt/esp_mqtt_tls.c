/*
 * esp_mqtt_tls.c
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */

#include "esp_mqtt_tls.h"

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			ESP_ERROR_CHECK(esp_wifi_connect())
			;
			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
			break;
		default:
			break;
	}
	return ESP_OK;
}
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	// your_context_t *context = event->context;
	esp_mqtt_client_handle_t client = event->client;
	ESP_ERROR_CHECK(spi_master_config());

	int ret;
	char* buf;
	char macbuf[17];
	double sensor_data;
	float temp;
	cJSON* msg;

	esp_base_mac_addr_set(mac);
	esp_efuse_read_mac(mac);
	sprintf(macbuf, "%x %x %x %x %x %x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	//macbuf = (char *) mac;

	while (1) {
		msg = cJSON_CreateObject();
		temp = (temprature_sens_read() - 32) / 1.8;
		ret = spi_master_read_sensor(&sensor_data);

		printf("Sensordata: %f\n", sensor_data);

		cJSON_AddStringToObject(msg, "mac", macbuf);
		cJSON_AddNumberToObject(msg, "value", sensor_data);
		cJSON_AddNumberToObject(msg, "Temperature", temp);

		buf = cJSON_Print(msg);

		if (status) {
			if (ret == ESP_ERR_TIMEOUT) {
				ESP_LOGE(TAG, "Communication Timeout");
			} else if (ret == ESP_OK) {
				esp_mqtt_client_publish(client, "device/id1/data", buf, 0, 0, 0);
			} else {
				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
			}
		}
		ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
		cJSON_Delete(msg);
		free(buf);
	}

	return ESP_OK;
}

void wifi_init(void)
{
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
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

void mqtt_app_start(void)
{
	const esp_mqtt_client_config_t mqtt_cfg = {
			.host = CONFIG_MQTT_SERVER,
			.port = CONFIG_MQTT_PORT,
			.event_handle = mqtt_event_handler,
			.transport = MQTT_TRANSPORT_OVER_SSL,
			.cert_pem = (const char *) ca_pem_start,
			.username = CONFIG_MQTT_USER,
			.password = CONFIG_MQTT_PASS,
			.client_id = "ESP_MQTT", };

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
}
