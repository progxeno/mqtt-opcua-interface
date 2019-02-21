///*
// * mbedtls_mqtt_sub.c
// *
// *  Created on: 14.02.2019
// *      Author: miosga.mario
// */
//
//
//uint8_t str2unit8(uint8_t *out, const char *s) {
//    char *end;
//    if (s[0] == '\0')
//        return 1;
//    errno = 0;
//    long l = strtol(s, &end, 10);
//    if (l > 255)
//        return 1;
//    if (l < 0)
//        return 1;
//    if (*end != '\0')
//        return 1;
//    *out = (uint8_t)l;
//    return 0;
//}
//
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-function"
//static void mqtt_message_handler(MessageData *md) {
//	uint8_t duty,gpio,ret;
//        char gpoinum[2];
//        char dutynum[255];
//
//	ESP_LOGI(TAG, "Topic received!: %.*s %.*s", md->topicName->lenstring.len, md->topicName->lenstring.data, md->message->payloadlen, (char*)md->message->payload);
//        gpoinum[0]=*(md->topicName->lenstring.data+md->topicName->lenstring.len-1);
//        gpoinum[1]='\0';
//        sprintf(dutynum,"%.*s",md->message->payloadlen, (char*)md->message->payload);
//	ret=str2unit8(&gpio,(const char *)gpoinum);
//	if (ret!=0) {
//		gpio=0;
//	}
//	if (gpio>=led_cnt) {
//		gpio=led_cnt-1;
//	}
//	str2unit8(&duty,(const char *)dutynum);
//	if (ret!=0) {
//		duty=0;
//	}
//
//	ESP_LOGI(TAG, "setLED!: %d %d %d %s %d", led_pin[gpio], gpio, duty,gpoinum,md->topicName->lenstring.len);
// 	setLED(led_pin[gpio],gpio,duty);
//}
//
//#pragma GCC diagnostic pop
//
//static void mqtt_task(void *pvParameters)
//{
//	int ret;
//	Network network;
//
//    while(1) {
//		ESP_LOGD(TAG, "Start MQTT Task ...");
//
//		MQTTClient client;
//		NetworkInit(&network);
//		network.websocket = MQTT_WEBSOCKET;
//
//		ESP_LOGD(TAG,"NetworkConnect %s:%d ...",MQTT_SERVER,MQTT_PORT);
//		ret = NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
//		if (ret != 0) {
//			ESP_LOGI(TAG, "NetworkConnect not SUCCESS: %d", ret);
//			goto exit;
//		}
//		ESP_LOGD(TAG,"MQTTClientInit  ...");
//		MQTTClientInit(&client, &network,
//			2000,            // command_timeout_ms
//			mqtt_sendBuf,         //sendbuf,
//			MQTT_BUF_SIZE, //sendbuf_size,
//			mqtt_readBuf,         //readbuf,
//			MQTT_BUF_SIZE  //readbuf_size
//		);
//
//		char buf[30];
//		MQTTString clientId = MQTTString_initializer;
//#if defined(MBEDTLS_MQTT_DEBUG)
//        sprintf(buf, "ESP32MQTT");
//#else
//        sprintf(buf, "ESP32MQTT%08X",esp_random());
//#endif
//		ESP_LOGI(TAG,"MQTTClientInit  %s",buf);
//        clientId.cstring = buf;
//
//    	MQTTString username = MQTTString_initializer;
//    	username.cstring = MQTT_USER;
//
//    	MQTTString password = MQTTString_initializer;
//    	password.cstring = MQTT_PASS;
//
//		MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
//		data.clientID          = clientId;
//		data.willFlag          = 0;
//		data.MQTTVersion       = 4; // 3 = 3.1 4 = 3.1.1
//		data.keepAliveInterval = 5;
//		data.cleansession      = 1;
//		data.username          = username;
//		data.password          = password;
//
//		ESP_LOGI(TAG,"MQTTConnect  ...");
//		ret = MQTTConnect(&client, &data);
//		if (ret != SUCCESS) {
//			ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
//			goto exit;
//		}
//
//		ESP_LOGI(TAG, "MQTTSubscribe  ...");
//		ret = MQTTSubscribe(&client, "esp32/bulb1/#", QOS0, mqtt_message_handler);
//		if (ret != SUCCESS) {
//			ESP_LOGI(TAG, "MQTTSubscribe: %d", ret);
//			goto exit;
//		}
//		ESP_LOGI(TAG, "MQTTYield  ...");
//		while(1) {
//			ret = MQTTYield(&client, (data.keepAliveInterval+1)*1000);
//			if (ret != SUCCESS) {
//				ESP_LOGI(TAG, "MQTTYield: %d", ret);
//				goto exit;
//			}
//		}
//		exit:
//			MQTTDisconnect(&client);
//			NetworkDisconnect(&network);
//			ESP_LOGI(TAG, "Starting again!");
//    }
//    esp_task_wdt_delete();
//    vTaskDelete(NULL);
// }
