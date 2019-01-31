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

	const esp_mqtt_client_config_t mqtt_cfg = configureClient(macAdr);

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
//	ESP_ERROR_CHECK(esp_mqtt_client_start(client));
	esp_err_t retval = esp_mqtt_client_start(client);

	printf("error: %s\n", esp_err_to_name(retval));

	sendOnlineMsg(client, macAdr);

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
		char * sendTemp = malloc(sizeof(char) * 5);
		char * sendSensorData = malloc(sizeof(char) * 5);
#ifdef SRC_DRIVER_PRSB25_H_
		ret = spi_master_read_sensor(&sensor_data);
		sprintf(sendSensorData, "%.3f", ((signed long)(sensor_data * 1000) * 0.001f));
#elif defined DRIVER_MB1222_H_
		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data);
		sprintf(sendSensorData, "%i", sensor_data);
#endif
		sprintf(sendTemp, "%.2f", ((signed long)(temp * 100) * 0.01f));
		free(sendTemp);
		free(sendSensorData);
		cJSON_AddStringToObject(jsonMsg, "ID", macAdr);
		cJSON_AddStringToObject(jsonMsg, "Value", sendSensorData);
		cJSON_AddStringToObject(jsonMsg, "Temperature", sendTemp);

		mqttMsg = cJSON_Print(jsonMsg);

		if (ret == ESP_ERR_TIMEOUT) {
			ESP_LOGE(TAG, "Communication Timeout");
		} else if (ret == ESP_OK) {
			esp_mqtt_client_publish(client, "device/id1/data", mqttMsg, 0, 0, 0);
			printf("Sensordata: %s\n", sendSensorData);
		} else {
			//ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
		}

		//ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
		cJSON_Delete(jsonMsg);
		free(mqttMsg);
		free(sendTemp);
		free(sendSensorData);
	}
	free(macAdr);

}

void sendOnlineMsg(esp_mqtt_client_handle_t client, char *macAdr){

	cJSON* jsonOnline = cJSON_CreateObject();
		char* onlineMsg;
		cJSON_AddStringToObject(jsonOnline, "ID", macAdr);
		cJSON_AddStringToObject(jsonOnline, "Publisher", "Online");
		onlineMsg = cJSON_Print(jsonOnline);

		int retval = esp_mqtt_client_publish(client, "device/online", onlineMsg, 0, 0, 0);

		if (retval != 0) {
			int i = 1;
			while (retval != 0) {
				sleep(1);
				i++;
				ESP_LOGW(TAG, "Attempt %i", i);
				retval = esp_mqtt_client_publish(client, "device/online", onlineMsg, 0, 0, 0);
			}
			if (retval != 0) {
				ESP_LOGE(TAG, "Failed to Connect to MQTT Broker %s:%d", CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
			}
		}

		cJSON_Delete(jsonOnline);
		free(onlineMsg);

}

esp_mqtt_client_config_t configureClient(char *macAdr){

	cJSON* jsonLW = cJSON_CreateObject();
		char* lwMsg;
		cJSON_AddStringToObject(jsonLW, "ID", macAdr);
		cJSON_AddStringToObject(jsonLW, "Publisher", "Offline");
		lwMsg = cJSON_Print(jsonLW);

		const esp_mqtt_client_config_t mqtt_cfg = {
				.host = CONFIG_MQTT_SERVER,
				.port = CONFIG_MQTT_PORT,
				.transport = MQTT_TRANSPORT_OVER_SSL,
				.cert_pem = (const char *) ca_pem_start,
				.username = CONFIG_MQTT_USER,
				.password = CONFIG_MQTT_PASS,
				.client_id = macAdr,
				.keepalive = 10,
				.lwt_topic = "device/offline",
				.lwt_qos = 0,
				.lwt_retain = 1,
				.lwt_msg = lwMsg,};

		cJSON_Delete(jsonLW);
		//free(lwMsg);
		printf("clientID: %s\n", mqtt_cfg.client_id);

		return mqtt_cfg;
}
