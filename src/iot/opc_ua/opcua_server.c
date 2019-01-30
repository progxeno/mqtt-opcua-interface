/*
 * opcua_server.c
 *
 *  Created on: 11.01.2019
 *      Author: miosga.mario
 */

#include "opcua_server.h"

void opcua_server_task(void *pvParameter)
{
	ESP_LOGI(TAG, "Fire up OPC UA Server.");
	config = UA_ServerConfig_new_default();

	//Set the connection config
	UA_ConnectionConfig connectionConfig;
	connectionConfig.recvBufferSize = 16384;
	connectionConfig.sendBufferSize = 16384;
	connectionConfig.maxMessageSize = 16384;

	UA_ServerNetworkLayer nl = UA_ServerNetworkLayerTCP(connectionConfig, 4840,
	NULL);

	//Set Discovery URL
	UA_String esp32url = UA_String_fromChars("opc.tcp://10.0.0.100:4840/");
	config->networkLayers = &nl;
	config->networkLayersSize = 1;
	config->networkLayers[0].discoveryUrl = UA_STRING("opc.tcp://esp32:4840");

	config->applicationDescription.discoveryUrls = &esp32url;
	config->applicationDescription.discoveryUrlsSize = 2;
	config->applicationDescription.applicationUri = UA_STRING("urn:SIMATIC.S7-1500.OPC-UA.Application:Mete");
	config->applicationDescription.applicationName = UA_LOCALIZEDTEXT("en-US", "ESP32Server");
	config->applicationDescription.applicationType = UA_APPLICATIONTYPE_SERVER;
	//config->applicationDescription.gatewayServerUri = UA_STRING("192.168.0.1");
	UA_ServerConfig_set_customHostname(config, UA_STRING("esp32"));
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

void addTemperatureNode(UA_Server *server)
{
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	//TODO: Temperature value should be read with a cycle and parsed into variable attr value.
	UA_Float ambientTemperature = (temprature_sens_read() - 32) / 1.8; //ReadTemperature(4);
	UA_Variant_setScalar(&attr.value, &ambientTemperature, &UA_TYPES[UA_TYPES_FLOAT]);

	attr.description = UA_LOCALIZEDTEXT("en-US", "Ambient Temperature in C");
	attr.displayName = UA_LOCALIZEDTEXT("en-US", "Ambient Temperature in C");
	attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
	attr.accessLevel = UA_ACCESSLEVELMASK_READ;

	UA_NodeId temperatureNodeId = UA_NODEID_STRING(1, "Temperature");
	UA_QualifiedName temperatureNodeName = UA_QUALIFIEDNAME(1, "Temperature Value");
	UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
	UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);

	UA_Server_addVariableNode(server, temperatureNodeId, parentNodeId, parentReferenceNodeId, temperatureNodeName,
								UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL,
								NULL);
}

