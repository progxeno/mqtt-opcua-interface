/*
 * opcua_server.c
 *
 *  Created on: 11.01.2019
 *      Author: miosga.mario
 */

#include "opcua_server.h"

void sensor_task(void *pvParameter) {
	//TODO: For now it only reads temperature once in parallel with opcua_task creation. Change this behaviour and make temperature dynamic.
	temperature = ReadTemperature(4);
	ESP_LOGI("Sensor_Task", "Temperature read from the sensor: %f", temperature);
	vTaskDelete(NULL);
}

void opcua_task(void *pvParameter) {
	ESP_LOGI(TAG, "Fire up OPC UA Server.");
	//config = UA_ServerConfig_new_customBuffer(4840, NULL, 8192, 8192);
	config = UA_ServerConfig_new_default();

	//Set the connection config
	UA_ConnectionConfig connectionConfig;
	connectionConfig.recvBufferSize = 32768;
	connectionConfig.sendBufferSize = 32768;

	UA_ServerNetworkLayer nl = UA_ServerNetworkLayerTCP(connectionConfig, 4840,
	NULL);

	//Set Discovery URL
	UA_String esp32url = UA_String_fromChars("opc.tcp://10.0.0.10:4840/");
	config->networkLayers = &nl;
	config->networkLayersSize = 1;
	config->networkLayers[0].discoveryUrl = UA_STRING("opc.tcp://espressif:4840");

	config->applicationDescription.discoveryUrls = &esp32url;
	config->applicationDescription.discoveryUrlsSize = 2;
	config->applicationDescription.applicationUri = UA_STRING("urn:SIMATIC.S7-1500.OPC-UA.Application:Mete");
	config->applicationDescription.applicationName = UA_LOCALIZEDTEXT("en-US", "ESP32Server");
	config->applicationDescription.applicationType = UA_APPLICATIONTYPE_SERVER;
	//config->applicationDescription.gatewayServerUri = UA_STRING("192.168.0.1");
	UA_ServerConfig_set_customHostname(config, UA_STRING("espressif"));
	UA_Server *server = UA_Server_new(config);

	addTemperatureNode(server);

	UA_Server_run_startup(server);
	UA_Boolean waitInternal = false;
	while (running) {
		UA_UInt16 timeout = UA_Server_run_iterate(server, waitInternal);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = timeout * 1000;
		select(0, NULL, NULL, NULL, &tv);
	}

	ESP_LOGI(TAG, "Now going to stop the server.");
	UA_Server_delete(server);
	UA_ServerConfig_delete(config);
	nl.deleteMembers(&nl);
	ESP_LOGI("OPC_TASK", "opcua_task going to return");
	vTaskDelete(NULL);
}

static void addTemperatureNode(UA_Server *server) {
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	//TODO: Temperature value should be read with a cycle and parsed into variable attr value.
	UA_Int32 ambientTemperature = temperature; //ReadTemperature(4);
	UA_Variant_setScalar(&attr.value, &ambientTemperature, &UA_TYPES[UA_TYPES_INT32]);

	attr.description = UA_LOCALIZEDTEXT("en-US", "Ambient Temperature in C");
	attr.displayName = UA_LOCALIZEDTEXT("en-US", "Ambient Temperature in C");
	attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	attr.accessLevel = UA_ACCESSLEVELMASK_READ;

	UA_NodeId temperatureNodeId = UA_NODEID_STRING(1, "Temperature");
	UA_QualifiedName temperatureNodeName = UA_QUALIFIEDNAME(1, "Temperature Value");
	UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
	UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);

	UA_Server_addVariableNode(server, temperatureNodeId, parentNodeId, parentReferenceNodeId, temperatureNodeName,
								UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL,
								NULL);
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
			ESP_ERROR_CHECK(esp_wifi_connect())
			;
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
			ESP_LOGI(TAG, "Got IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			// TODO: Here I create task that start a OPC UA Server
			xTaskCreate(&opcua_task, "opcua_task", 1024 * 8, NULL, 1, NULL);
			ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
			ESP_ERROR_CHECK(esp_wifi_connect())
			;
			break;
		default:
			break;
	}
	return ESP_OK;
}

static void wifi_scan(void) {
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	wifi_config_t wifi_config = {
			.sta = {
					.ssid = DEFAULT_SSID,
					.password =
					DEFAULT_PWD }, };

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "espressif");
}

void app_main() {

	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	wifi_scan();
	//xTaskCreate(&opcua_task, "opcua_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

