/*
 * mbedtls_mqtt.c
 *
 *  Created on: 06.12.2018
 *      Author: Mario
 */
#include "mbedtls_mqtt.h"

 void initialise_wifi(void) {
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK (esp_wifi_set_storage(WIFI_STORAGE_RAM) );wifi_config_t
	wifi_config = { .sta = { .ssid = CONFIG_WIFI_SSID, .password = CONFIG_WIFI_PASSWORD, }, };
	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...",
			wifi_config.sta.ssid);
	ESP_ERROR_CHECK (esp_wifi_set_mode(WIFI_MODE_STA) );ESP_ERROR_CHECK
	(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK (esp_wifi_start() );}

 void mqtt_task(void *pvParameters) {
	Network network;
	ESP_ERROR_CHECK(i2c_master_init());

	int ret;
	char buf[10];
	uint8_t sensor_data_h, sensor_data_l;
	uint16_t sensor_data;
	/* Wait for the callback to set the CONNECTED_BIT in the
	 event group.
	 */
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
			false, true, portMAX_DELAY);
	ESP_LOGI(TAG, "Connected to AP");

	ESP_LOGI(TAG, "Start MQTT Task ...");

	MQTTClient client;
	NetworkInit(&network);
	network.websocket = MQTT_WEBSOCKET;

	ESP_LOGI(TAG, "NetworkConnect %s:%d ...", MQTT_SERVER, MQTT_PORT);
	NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
	ESP_LOGI(TAG, "MQTTClientInit  ...");
	MQTTClientInit(&client, &network, 2000,            // command_timeout_ms
			mqtt_sendBuf,         //sendbuf,
			MQTT_BUF_SIZE, //sendbuf_size,
			mqtt_readBuf,         //readbuf,
			MQTT_BUF_SIZE  //readbuf_size
			);

	MQTTString clientId = MQTTString_initializer;
	clientId.cstring = "MBEDTLS_MQTT";
	MQTTString username = MQTTString_initializer;
	username.cstring = MQTT_USER;
	MQTTString password = MQTTString_initializer;
	password.cstring = MQTT_PASS;

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID = clientId;
	data.willFlag = 0;
	data.MQTTVersion = 4; // 3 = 3.1 4 = 3.1.1
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	data.username = username;
	data.password = password;

	ESP_LOGI(TAG, "MQTTConnect  ...");
	ret = MQTTConnect(&client, &data);
	if (ret != SUCCESS) {
		ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
	}
	while (1) {

		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data_h,
				&sensor_data_l);
		sensor_data = (uint16_t) sensor_data_h << 8 | sensor_data_l;
		sprintf(buf, "%u", sensor_data);

		MQTTMessage message;
		//	ESP_LOGI(TAG, "MQTTPublish  ... %s",(uint8_t *) buf);

		message.qos = QOS0;
		message.retained = false;
		message.dup = false;
		message.payload = (uint8_t *) buf;
		message.payloadlen = strlen(buf) + 1;

		if (status) {
			if (ret == ESP_ERR_TIMEOUT) {
				ESP_LOGE(TAG, "I2C Timeout");
			} else if (ret == ESP_OK) {
				MQTTPublish(&client, "device/id1/data", &message);
			} else {
				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ",
						esp_err_to_name(ret));
			}
		}
	}
}
