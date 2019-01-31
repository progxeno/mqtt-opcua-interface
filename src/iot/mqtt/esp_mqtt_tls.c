/*
 * esp_mqtt_tls.c
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */

#include "esp_mqtt_tls.h"

void mqtt_esp_task(void *pvParameters)
{
#ifdef SRC_DRIVER_PRSB25_H_
	ESP_ERROR_CHECK(spi_master_config());
	double sensor_data;
#elif defined DRIVER_MB1222_H_
	ESP_ERROR_CHECK(i2c_master_init());
	uint16_t sensor_data;
#endif

	int ret;
	char* mqttMsg;
	char *macAdr = malloc(sizeof(char) * 13);
	double temp;
	cJSON* jsonMsg;
	int connected;
	int i = 1;

	esp_base_mac_addr_set(mac);
	esp_efuse_mac_get_default(mac);
	snprintf(macAdr, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	const esp_mqtt_client_config_t mqtt_cfg = configureClient(macAdr);

	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_err_t retval = esp_mqtt_client_start(client);

	printf("error: %s\n", esp_err_to_name(retval));

	connected = sendOnlineMsg(client, macAdr);
	while (1) {

		if (connected != 0) {
			i++;
			ESP_LOGW(TAG, "Attempt %i", i);
			connected = sendOnlineMsg(client, macAdr);
			sleep(1);

		} else {
			jsonMsg = cJSON_CreateObject();
			temp = round(((temprature_sens_read() - 32) / 1.8) * 100.0) / 100.0;

#ifdef SRC_DRIVER_PRSB25_H_
			ret = spi_master_read_sensor(&sensor_data);
			sensor_data = round(sensor_data * 1000.0) / 1000.0;
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

				connected = esp_mqtt_client_publish(client, "device/id1/data", mqttMsg, 0, 0, 0);
#ifdef SRC_DRIVER_PRSB25_H_
				printf("Sensordata: %.3f\n", sensor_data);
#elif defined DRIVER_MB1222_H_
				printf("Sensordata: %i\n", sensor_data);
#endif

			} else {
				//ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
			}

			//ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
			cJSON_Delete(jsonMsg);
			free(mqttMsg);
		}
	}
	free(macAdr);

}

int sendOnlineMsg(esp_mqtt_client_handle_t client, char *macAdr)
{

	cJSON* jsonOnline = cJSON_CreateObject();
	char* onlineMsg;
	cJSON_AddStringToObject(jsonOnline, "ID", macAdr);
	cJSON_AddStringToObject(jsonOnline, "Publisher", "Online");
	onlineMsg = cJSON_Print(jsonOnline);

	int retval = esp_mqtt_client_publish(client, "device/online", onlineMsg, 0, 0, 0);

	cJSON_Delete(jsonOnline);
	free(onlineMsg);
	return retval;
}

esp_mqtt_client_config_t configureClient(char *macAdr)
{

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
			.keepalive = 60,
			.lwt_topic = "device/offline",
			.lwt_qos = 0,
			.lwt_retain = 1,
			.lwt_msg = lwMsg, };

	cJSON_Delete(jsonLW);
	printf("clientID: %s\n", mqtt_cfg.client_id);

	return mqtt_cfg;
}

