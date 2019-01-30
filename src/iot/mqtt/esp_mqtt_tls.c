/*
 * esp_mqtt_tls.c
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */

#include "esp_mqtt_tls.h"

void mqtt_esp_task(void *pvParameters)
{
	char *macAdr = malloc(sizeof(char) * 13);
	esp_base_mac_addr_set(mac);
	esp_efuse_mac_get_default(mac);
	snprintf(macAdr, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	const esp_mqtt_client_config_t mqtt_cfg = {
			.host = CONFIG_MQTT_SERVER,
			.port = CONFIG_MQTT_PORT,
			.transport = MQTT_TRANSPORT_OVER_SSL,
			.cert_pem = (const char *) ca_pem_start,
			.username = CONFIG_MQTT_USER,
			.password = CONFIG_MQTT_PASS,
			.client_id = macAdr, };
	printf("clientID: %s\n", mqtt_cfg.client_id);

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	ESP_ERROR_CHECK(esp_mqtt_client_start(client));

#ifdef SRC_DRIVER_PRSB25_H_
	ESP_ERROR_CHECK(spi_master_config());
	double sensor_data;
#elif defined DRIVER_MB1222_H_
	ESP_ERROR_CHECK(i2c_master_init());
	uint16_t sensor_data;
#endif

	int ret;
	char* mqttMsg;
	float temp;
	cJSON* jsonMsg;

	while (1) {
		jsonMsg = cJSON_CreateObject();
		temp = (temprature_sens_read() - 32) / 1.8;

#ifdef SRC_DRIVER_PRSB25_H_
		ret = spi_master_read_sensor(&sensor_data);
#elif defined DRIVER_MB1222_H_
		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data);
#endif

		cJSON_AddStringToObject(jsonMsg, "ID", macAdr);
		cJSON_AddNumberToObject(jsonMsg, "Value", sensor_data);
		cJSON_AddNumberToObject(jsonMsg, "Temperature", temp);

		mqttMsg = cJSON_Print(jsonMsg);

		if (ret == ESP_ERR_TIMEOUT) {
			ESP_LOGE(TAG, "Communication Timeout");
		} else if (ret == ESP_OK) {
			esp_mqtt_client_publish(client, "device/id1/data", mqttMsg, 0, 0, 0);
#ifdef SRC_DRIVER_PRSB25_H_
			printf("Sensordata: %f\n", sensor_data);
#elif defined DRIVER_MB1222_H_
			printf("Sensordata: %i\n", sensor_data);
#endif
		} else {
			ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
		}

		ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
		cJSON_Delete(jsonMsg);
		free(mqttMsg);
	}
	free(macAdr);

}
