/*
 * opcua_pubsub.c
 *
 *  Created on: 09.01.2019
 *      Author: miosga.mario
 */

#include "opcua_pubsub.h"

void addPubSubConnection(UA_Server *server)
{
	/* Details about the connection configuration and handling are located
	 * in the pubsub connection tutorial */
	UA_PubSubConnectionConfig connectionConfig;
	memset(&connectionConfig, 0, sizeof(connectionConfig));
	connectionConfig.name = UA_STRING("UDP-UADP Connection 1");
	connectionConfig.transportProfileUri = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
	connectionConfig.enabled = UA_TRUE;
	UA_NetworkAddressUrlDataType networkAddressUrl = {
			UA_STRING_NULL,
			UA_STRING("opc.udp://224.0.0.22:4840/") };
	UA_Variant_setScalar(&connectionConfig.address, &networkAddressUrl, &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
	connectionConfig.publisherId.numeric = UA_UInt32_random();
	UA_StatusCode addPBConnStat = UA_Server_addPubSubConnection(server, &connectionConfig, &connectionIdent);
	printf("Add PubSub Connection Status Code: %d\n", addPBConnStat);
}

/**
 * **PublishedDataSet handling**
 * The PublishedDataSet (PDS) and PubSubConnection are the toplevel entities and can exist alone. The PDS contains
 * the collection of the published fields.
 * All other PubSub elements are directly or indirectly linked with the PDS or connection.
 */
void addPublishedDataSet(UA_Server *server)
{
	/* The PublishedDataSetConfig contains all necessary public
	 * informations for the creation of a new PublishedDataSet */
	UA_PublishedDataSetConfig publishedDataSetConfig;
	memset(&publishedDataSetConfig, 0, sizeof(UA_PublishedDataSetConfig));
	publishedDataSetConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS;
	publishedDataSetConfig.name = UA_STRING("Demo PDS");
	/* Create new PublishedDataSet based on the PublishedDataSetConfig. */
	UA_AddPublishedDataSetResult s = UA_Server_addPublishedDataSet(server, &publishedDataSetConfig, &publishedDataSetIdent);
	printf("Add Publish DateSet Status Code: %d\n", s.addResult);

}

/**
 * **DataSetField handling**
 * The DataSetField (DSF) is part of the PDS and describes exactly one published field.
 */
void addDataSetField(UA_Server *server)
{

	/* Add a field to the previous created PublishedDataSet */
	UA_NodeId dataSetFieldIdent;
	UA_DataSetFieldConfig dataSetFieldDate;
	UA_DataSetFieldConfig dataSetFieldTemp;
//	memset(&dataSetFieldDate, 0, sizeof(UA_DataSetFieldConfig));
//	dataSetFieldDate.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
//	dataSetFieldDate.field.variable.fieldNameAlias = UA_STRING("Server localtime");
//	dataSetFieldDate.field.variable.promotedField = UA_FALSE;
//	dataSetFieldDate.field.variable.publishParameters.publishedVariable = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
//	dataSetFieldDate.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;
//	UA_Server_addDataSetField(server, publishedDataSetIdent, &dataSetFieldDate, &dataSetFieldIdent);
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	attr.minimumSamplingInterval = 0.000000;
	attr.userAccessLevel = 3;
	attr.accessLevel = 3;
	attr.valueRank = -1;
	attr.dataType = UA_NODEID_NUMERIC(0, 12); //6 for INT32
	UA_String classVar = UA_STRING("Temperature: 21 C");
	UA_Variant_setScalar(&attr.value, &classVar, &UA_TYPES[UA_TYPES_STRING]);
//
	UA_StatusCode addNodeStat = UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, UA_NODEID_NUMERIC(1, 6001),
														UA_NODEID_NUMERIC(0, UA_NS0ID_PUBLISHSUBSCRIBE), UA_NODEID_NUMERIC(0, 47),
														UA_QUALIFIEDNAME(1, "Test"), UA_NODEID_NUMERIC(0, 63), (const UA_NodeAttributes*) &attr,
														&UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],
														NULL,
														&createdNodeId);

	parseTemperature(server, createdNodeId);
	memset(&dataSetFieldTemp, 0, sizeof(UA_DataSetFieldConfig));
	dataSetFieldTemp.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
	dataSetFieldTemp.field.variable.fieldNameAlias = UA_STRING("Server localtime");
	dataSetFieldTemp.field.variable.promotedField = UA_FALSE;
	dataSetFieldTemp.field.variable.publishParameters.publishedVariable = createdNodeId;
	dataSetFieldTemp.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;

	UA_DataSetFieldResult addDataSetFieldStat = UA_Server_addDataSetField(server, publishedDataSetIdent, &dataSetFieldTemp, &dataSetFieldIdent);


	//	/* Add a field to the previous created PublishedDataSet */
	//free(buf);
	//ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());

//	UA_String value = *(UA_String*)createdNodeId.identifier.string.data;
//	printf("Output string: " UA_PRINTF_STRING_FORMAT "\n", UA_PRINTF_STRING_DATA(value));
//	printf("\t Output string2: \"%.*s\"",(int)value.length, value.data);

}

/**
 * **WriterGroup handling**
 * The WriterGroup (WG) is part of the connection and contains the primary configuration
 * parameters for the message creation.
 */
void addWriterGroup(UA_Server *server)
{
	/* Now we create a new WriterGroupConfig and add the group to the existing PubSubConnection. */
	UA_WriterGroupConfig writerGroupConfig;
	memset(&writerGroupConfig, 0, sizeof(UA_WriterGroupConfig));
	writerGroupConfig.name = UA_STRING("Demo WriterGroup");
	writerGroupConfig.publishingInterval = 100;
	writerGroupConfig.enabled = UA_FALSE;
	writerGroupConfig.writerGroupId = 100;
	writerGroupConfig.encodingMimeType = UA_PUBSUB_ENCODING_UADP;
	/* The configuration flags for the messages are encapsulated inside the
	 * message- and transport settings extension objects. These extension objects
	 * are defined by the standard. e.g. UadpWriterGroupMessageDataType */
	UA_Server_addWriterGroup(server, connectionIdent, &writerGroupConfig, &writerGroupIdent);
}

/**
 * **DataSetWriter handling**
 * A DataSetWriter (DSW) is the glue between the WG and the PDS. The DSW is linked to exactly one
 * PDS and contains additional informations for the message generation.
 */
void addDataSetWriter(UA_Server *server)
{
	/* We need now a DataSetWriter within the WriterGroup. This means we must
	 * create a new DataSetWriterConfig and add call the addWriterGroup function. */
	UA_NodeId dataSetWriterIdent;
	UA_DataSetWriterConfig dataSetWriterConfig;
	memset(&dataSetWriterConfig, 0, sizeof(UA_DataSetWriterConfig));
	dataSetWriterConfig.name = UA_STRING("Demo DataSetWriter");

	esp_base_mac_addr_set(mac);
	esp_efuse_mac_get_default(mac);
	printf("MAC: %x %x %x %x %x %x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	dataSetWriterConfig.dataSetWriterId = (mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]);
	dataSetWriterConfig.keyFrameCount = 10;
	UA_Server_addDataSetWriter(server, writerGroupIdent, publishedDataSetIdent, &dataSetWriterConfig, &dataSetWriterIdent);
	printf("ID: %x\n", dataSetWriterConfig.dataSetWriterId);
}

void removeNode(UA_Server *server, UA_NodeId nodeId){
	UA_Server_deleteNode(server, nodeId, true);
	UA_NodeId_deleteMembers(&nodeId);

}

void parseTemperature(UA_Server *server, const UA_NodeId nodeid)
{
	float temp;
	char *buf = UA_malloc(sizeof(char) * 512);
	temp = (temprature_sens_read() - 32) / 1.8;
//	printf("Returned Temperature: %.6f\n", temp);
	snprintf(buf, 512, "%f", temp);
	//printf("Read Temperature : %s\n", buf);
	UA_String temperature = UA_String_fromChars(buf);
//	UA_free(&buf);

	//UA_String temperature = UA_STRING("Temperature as string!"); //Change here as read numeric temperature value
	UA_Variant value;
	UA_Variant_setScalar(&value, &temperature, &UA_TYPES[UA_TYPES_STRING]);
	UA_Server_writeValue(server, nodeid, value);

}

void opcua_task(void *pvParameter)
{
	UA_ServerConfig *config;
	ESP_LOGI(TAG, "Fire up OPC UA Server.");
	//config = UA_ServerConfig_new_customBuffer(4840, NULL, 8192, 8192);
	config = UA_ServerConfig_new_default();

	/* Details about the connection configuration and handling are located in the pubsub connection tutorial */
	config->pubsubTransportLayers = (UA_PubSubTransportLayer *) UA_malloc(sizeof(UA_PubSubTransportLayer));
	if (!config->pubsubTransportLayers) {
		UA_ServerConfig_delete(config);
		return;
	}

	config->customHostname = UA_STRING("ESP32");
	UA_String esp32url = UA_String_fromChars("opc.udp://raspberrypi:4840");
	config->applicationDescription.discoveryUrls = &esp32url;
	config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
	config->pubsubTransportLayersSize++;
	UA_Server *server = UA_Server_new(config);

	addPubSubConnection(server);
	addPublishedDataSet(server);
	addDataSetField(server);
	addWriterGroup(server);
	addDataSetWriter(server);

	//UA_Server_run(server, &running);
	UA_Server_run_startup(server);

	UA_Boolean waitInternal = false;
	while (running) {
		//UA_Server_run(server, &running);
		UA_UInt16 timeout = UA_Server_run_iterate(server, waitInternal);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = timeout * 10000;
		select(0, NULL, NULL, NULL, &tv);
//		parseTemperature(server, createdNodeId);
//		multi_heap_info_t heapInfo;

//		printf("         %10s %10s %10s %10s %13s %11s %12s\n", "Free", "Allocated", "Largest", "Minimum", "Alloc Blocks", "Free Blocks", "Total Blocks");
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_EXEC);
//		printf("EXEC     %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_32BIT);
//		printf("32BIT    %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_8BIT);
//		printf("8BIT     %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_DMA);
//		printf("DMA      %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_SPIRAM);
//		printf("SPISRAM  %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_INTERNAL);
//		printf("INTERNAL %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
//		heap_caps_get_info(&heapInfo, MALLOC_CAP_DEFAULT);
//		printf("DEFAULT  %10d %10d %10d %10d %13d %11d %12d\n", heapInfo.total_free_bytes, heapInfo.total_allocated_bytes, heapInfo.largest_free_block, heapInfo.minimum_free_bytes, heapInfo.allocated_blocks, heapInfo.free_blocks, heapInfo.total_blocks);
		ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", xPortGetFreeHeapSize());// esp_get_free_heap_size());

//		ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());
	}

	ESP_LOGI(TAG, "Now going to stop the server.");
	UA_Server_delete(server);
	UA_ServerConfig_delete(config);
	//nl.deleteMembers(&nl);
	ESP_LOGI(TAG, "opcua_task going to return");
	vTaskDelete(NULL);
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id) {
		case SYSTEM_EVENT_STA_START:
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
			ESP_ERROR_CHECK(esp_wifi_connect());
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
			ESP_LOGI(TAG, "Got IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			// TODO: Here I create task that start a OPC UA Server
			xTaskCreate(&opcua_task, "opcua_task", 1024 * 8, NULL, 5, NULL);
			ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
			ESP_ERROR_CHECK(esp_wifi_connect());
			break;
		default:
			break;
	}
	return ESP_OK;
}

void wifi_scan(void)
{
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	wifi_config_t wifi_config = {
			.sta = {
					.ssid = CONFIG_DEFAULT_SSID,
					.password = CONFIG_DEFAULT_PWD }, };

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

}
