/*
 * mbedtls_pub_mqtt.c
 *
 *  Created on: 09.04.2019
 *      Author: miosga.mario
 */

#include "mbedtls_pub_mqtt.h"
//char ptrTaskList[250];

void mqtt_mbedtls_pub_task(void *pvParameters)
{
	//vTaskEndScheduler();
	printf("MQTT running on Core: %i\n", xPortGetCoreID());

	while (1) {
		if (xSemaphore != NULL) {
			/* See if we can obtain the semaphore.  If the semaphore is not
			 available wait 10 ticks to see if it becomes free. */
			if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {

				int ret;
				char* mqttMsg;
				char *macAdr = malloc(sizeof(char) * 13);
				cJSON* jsonMsg;
				Network network;
				MQTTClient client;
				MQTTPacket_connectData data = MQTTPacket_connectData_initializer
				;

				setMac(macAdr);
				ESP_LOGI(TAG, "Start MQTT Task ...");

				NetworkInit(&network);
				network.websocket = MQTT_WEBSOCKET;

				configureClient(&data, macAdr);
				startClient(&client, &network, &data);
				sendOnlineMsg(client, macAdr);
				ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
				xSemaphoreGive(xSemaphore);
				while (1) {

					char RxBuffer[MaxQueueSize][MQTT_BUF_SIZE];
					/* See if we can obtain the semaphore.  If the semaphore is not
					 available wait 10 ticks to see if it becomes free. */
					if (pdTRUE == xQueueReceive(MyQueueHandleId, &RxBuffer, 100) && xSemaphoreTake(xSemaphore, (TickType_t) 100) == pdTRUE) {
						printf("recieved ID: %s\n", RxBuffer[2]);
						printf("recieved Value: %s\n", RxBuffer[1]);
						printf("recieved Temperature: %s\n", RxBuffer[0]);

						jsonMsg = cJSON_CreateObject();
						cJSON_AddStringToObject(jsonMsg, "ID", RxBuffer[2]);
						cJSON_AddStringToObject(jsonMsg, "Value", RxBuffer[1]);
						cJSON_AddStringToObject(jsonMsg, "Temperature", RxBuffer[0]);

						mqttMsg = cJSON_Print(jsonMsg);

						MQTTMessage message;

						message.qos = QOS0;
						message.retained = false;
						message.dup = false;
						message.payload = (uint16_t *) mqttMsg;
						message.payloadlen = strlen(mqttMsg) + 1;

						MQTTPublish(&client, "device/id1/data", &message);
						cJSON_Delete(jsonMsg);
						free(mqttMsg);
						xSemaphoreGive(xSemaphore);
					} else {
						xSemaphoreGive(xSemaphore);
						continue;
					}
				}
				vTaskDelay(10 / portTICK_RATE_MS);

			} else {
				xSemaphoreGive(xSemaphore);
				continue;
			}
		}
	}
}

void startClient(MQTTClient *client, Network *network, MQTTPacket_connectData *data)
{
	int ret;
	ESP_LOGI(TAG, "NetworkConnect %s:%d ...", CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
	int retval = NetworkConnect(network, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);

	if (retval != 0) {
		int i = 1;
		while (retval != 0 && i <= 100) {
			vTaskDelay(pdMS_TO_TICKS(1000));
			i++;
			ESP_LOGW(TAG, "Connection failed: %i ... Reconnecting", retval);
			ESP_LOGW(TAG, "Attempt %i", i);
//			vTaskList(ptrTaskList);
//			printf("Task  State   Prio    Stack    Num\n");
//			printf("**********************************\n");
//			printf(ptrTaskList);
//			printf("**********************************\n");
			retval = NetworkConnect(network, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);

		}
		if (retval != 0) {
			ESP_LOGE(TAG, "Failed to Connect to MQTT Broker %s:%d", CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
		}
	}

	ESP_LOGI(TAG, "MQTTClientInit  ...");
	MQTTClientInit(client, network, 2000, mqtt_sendBuf, MQTT_BUF_SIZE, mqtt_readBuf, MQTT_BUF_SIZE);

	ESP_LOGI(TAG, "MQTTConnect  ...");
	ret = MQTTConnect(client, data);
	if (ret != SUCCESS) {
		ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
	}

}

void setMac(char *macAdr)
{
	esp_base_mac_addr_set(mac);
	esp_efuse_mac_get_default(mac);
	snprintf(macAdr, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void sendOnlineMsg(MQTTClient client, char *macAdr)
{

	cJSON* jsonOnline = cJSON_CreateObject();
	char* onlineMsg;
	cJSON_AddStringToObject(jsonOnline, "ID", macAdr);
	cJSON_AddStringToObject(jsonOnline, "Publisher", "Online");

	onlineMsg = cJSON_Print(jsonOnline);

	MQTTMessage onlineMessage;

	onlineMessage.qos = QOS0;
	onlineMessage.retained = false;
	onlineMessage.dup = false;
	onlineMessage.payload = (uint16_t *) onlineMsg;
	onlineMessage.payloadlen = strlen(onlineMsg) + 1;

	MQTTPublish(&client, "device/online", &onlineMessage);
	cJSON_Delete(jsonOnline);
	free(onlineMsg);

}

void configureClient(MQTTPacket_connectData *data, char *macAdr)
{
	cJSON* jsonLW = cJSON_CreateObject();
	char* lwMsg;
	cJSON_AddStringToObject(jsonLW, "ID", macAdr);
	cJSON_AddStringToObject(jsonLW, "Publisher", "Offline");

	lwMsg = cJSON_Print(jsonLW);

	char buf[30];
	sprintf(buf, macAdr, esp_random());

	MQTTString clientId = MQTTString_initializer
	;
	clientId.cstring = buf;
	MQTTString username = MQTTString_initializer
	;
	username.cstring = CONFIG_MQTT_USER;
	MQTTString password = MQTTString_initializer
	;
	password.cstring = CONFIG_MQTT_PASS;

	data->clientID = clientId;
	data->MQTTVersion = 4; // 3 = 3.1 4 = 3.1.1
	data->keepAliveInterval = 60;
	data->cleansession = 1;
	data->username = username;
	data->password = password;
	data->willFlag = 1;
	data->will.topicName.cstring = "device/offline";
	data->will.qos = QOS0;
	data->will.retained = true;
	data->will.message.cstring = lwMsg;

	printf("clientID: %s\n", clientId.cstring);

	cJSON_Delete(jsonLW);

}

