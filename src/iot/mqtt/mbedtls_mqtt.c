/*
 * mbedtls_mqtt.c
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */
#include "mbedtls_mqtt.h"

void mqtt_mbedtls_task(void *pvParameters)
{
	Network network;
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
	float temp;
	cJSON* jsonMsg;

	esp_base_mac_addr_set(mac);
	esp_efuse_mac_get_default(mac);
	snprintf(macAdr, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	ESP_LOGI(TAG, "Start MQTT Task ...");

	MQTTClient client;
	NetworkInit(&network);
	network.websocket = MQTT_WEBSOCKET;

	ESP_LOGI(TAG, "NetworkConnect %s:%d ...", CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
	NetworkConnect(&network, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
	ESP_LOGI(TAG, "MQTTClientInit  ...");
	MQTTClientInit(&client, &network, 2000, mqtt_sendBuf, MQTT_BUF_SIZE, mqtt_readBuf, MQTT_BUF_SIZE);

	MQTTString clientId = MQTTString_initializer;
	clientId.cstring = macAdr;
	MQTTString username = MQTTString_initializer;
	username.cstring = CONFIG_MQTT_USER;
	MQTTString password = MQTTString_initializer;
	password.cstring = CONFIG_MQTT_PASS;

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID = clientId;
	data.willFlag = 0;
	data.MQTTVersion = 4; // 3 = 3.1 4 = 3.1.1
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	data.username = username;
	data.password = password;

	printf("clientID: %s\n", clientId.cstring);
	ESP_LOGI(TAG, "MQTTConnect  ...");
	ret = MQTTConnect(&client, &data);
	if (ret != SUCCESS) {
		ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
	}

	while (1) {

		jsonMsg = cJSON_CreateObject();
		temp = (temprature_sens_read() - 32) / 1.8;

#ifdef SRC_DRIVER_PRSB25_H_
		ret = spi_master_read_sensor(&sensor_data);
#elif defined DRIVER_MB1222_H_
		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data);
#endif

		cJSON_AddStringToObject(jsonMsg, "mac", macAdr);
		cJSON_AddNumberToObject(jsonMsg, "value", sensor_data);
		cJSON_AddNumberToObject(jsonMsg, "Temperature", temp);

		mqttMsg = cJSON_Print(jsonMsg);

		MQTTMessage message;
		//	ESP_LOGI(TAG, "MQTTPublish  ... %s",(uint8_t *) mqttMsg);

		message.qos = QOS0;
		message.retained = false;
		message.dup = false;
		message.payload = (uint16_t *) mqttMsg;
		message.payloadlen = strlen(mqttMsg) + 1;

		if (ret == ESP_ERR_TIMEOUT) {
			ESP_LOGE(TAG, "I2C Timeout");
		} else if (ret == ESP_OK) {
			MQTTPublish(&client, "device/id1/data", &message);
			printf("Sensor-data: %i\n", sensor_data);
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGW(TAG, "%s: CRC check Failed ", esp_err_to_name(ret));
		} else {
			ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
		}

		//ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
		cJSON_Delete(jsonMsg);
		free(mqttMsg);
	}
}
