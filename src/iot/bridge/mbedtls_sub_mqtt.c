/*
 * mbedtls_mqtt_sub.c
 *
 *  Created on: 20.02.2019
 *      Author: miosga.mario
 */
#include "mbedtls_sub_mqtt.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
void mqtt_message_handler(MessageData *md)
{
	ESP_LOGI(TAG, "Topic received!: %.*s %.*s", md->topicName->lenstring.len, md->topicName->lenstring.data, md->message->payloadlen,
				(char* ) md->message->payload);
	char TxBuffer[1][MQTT_BUF_SIZE];
	sprintf(TxBuffer[0], "%.*s count %i\n", md->message->payloadlen, (char*) md->message->payload, it);
	if (pdTRUE == xQueueSend(MyQueueHandleId, TxBuffer[0], 10) && xSemaphoreTake(xSemaphore, (TickType_t ) 100) == pdTRUE) {
//		printf("MQTT_SUB: Successfully sent the data\n");
		xSemaphoreGive(xSemaphore);

	}
	it++;

	vTaskDelay(10);

}
#pragma GCC diagnostic pop

void mqtt_mbedtls_sub_task(void *pvParameters)
{
	printf("MQTT running on Core: %i\n", xPortGetCoreID());
	int ret;
	char *macAdr = malloc(sizeof(char) * 13);
	Network network;
	MQTTClient client;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer
	;

	while (1) {
		if (xSemaphore != NULL) {
			/* See if we can obtain the semaphore.  If the semaphore is not
			 available wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
				ESP_LOGD(TAG, "Start MQTT Task ...");

				NetworkInit(&network);
				network.websocket = MQTT_WEBSOCKET;

				setMac(macAdr);
				configureClient(&data, macAdr);
				startClient(&client, &network, &data);
				xSemaphoreGive(xSemaphore);

				ESP_LOGI(TAG, "MQTTSubscribe  ...");
				ret = MQTTSubscribe(&client, "device/#", QOS0, mqtt_message_handler);
				if (ret != SUCCESS) {
					ESP_LOGI(TAG, "MQTTSubscribe: %d", ret);
					goto exit;
				}
				ESP_LOGI(TAG, "MQTTYield  ...");
				while (1) {
					ret = MQTTYield(&client, (data.keepAliveInterval + 1) * 1000);
					if (ret != SUCCESS) {
						ESP_LOGI(TAG, "MQTTYield: %d", ret);
						goto exit;
					}
				}
				exit: MQTTDisconnect(&client);
				NetworkDisconnect(&network);
				ESP_LOGI(TAG, "Starting again!");
			} else {
				continue;
			}
		} else {
			continue;
		}
	}
	vTaskDelete(NULL);
}

void setMac(char *macAdr)
{
	esp_base_mac_addr_set(mac);
	esp_efuse_mac_get_default(mac);
	snprintf(macAdr, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void configureClient(MQTTPacket_connectData *data, char *macAdr)
{
//	cJSON* jsonLW = cJSON_CreateObject();
//	char* lwMsg;
//	cJSON_AddStringToObject(jsonLW, "ID", macAdr);
//	cJSON_AddStringToObject(jsonLW, "Publisher", "Offline");
//
//	lwMsg = cJSON_Print(jsonLW);

	char buf[30];
	sprintf(buf, macAdr, esp_random());

	MQTTString clientId = MQTTString_initializer
	;
	clientId.cstring = buf;
	MQTTString username = MQTTString_initializer
	;
	username.cstring = "client";
	MQTTString password = MQTTString_initializer
	;
	password.cstring = "client@iot";

	data->clientID = clientId;
	data->MQTTVersion = 4; // 3 = 3.1 4 = 3.1.1
	data->keepAliveInterval = 60;
	data->cleansession = 1;
	data->username = username;
	data->password = password;
	data->willFlag = 0;
//	data->will.topicName.cstring = "device/offline";
//	data->will.qos = QOS0;
//	data->will.retained = true;
//	data->will.message.cstring = lwMsg;

	printf("clientID: %s\n", clientId.cstring);

//	cJSON_Delete(jsonLW);

}

void startClient(MQTTClient *client, Network *network, MQTTPacket_connectData *data)
{
	int ret;
	ESP_LOGI(TAG, "NetworkConnect %s:%d ...", CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
	int retval = NetworkConnect(network, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
//	vTaskDelay(pdMS_TO_TICKS(1000));
//	char ptrTaskList[250];
	if (retval != 0) {
		int i = 1;
		while (retval != 0 && i <= 10) {
			vTaskDelay(pdMS_TO_TICKS(1000));
			i++;
			ESP_LOGW(TAG, "Connection failed: %i ... Reconnecting", retval);
			ESP_LOGW(TAG, "Attempt %i", i);
			retval = NetworkConnect(network, CONFIG_MQTT_SERVER, CONFIG_MQTT_PORT);
//			vTaskList(ptrTaskList);
//			printf("Task  State   Prio    Stack    Num\n");
//			printf("**********************************\n");
//			printf(ptrTaskList);
//			printf("**********************************\n");
			ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

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

