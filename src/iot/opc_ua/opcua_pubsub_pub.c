/*
 * opcua_pubsub_pub.c
 *
 *  Created on: 12.03.2019
 *      Author: miosga.mario
 */

#include "opcua_pubsub_pub.h"

void opcua_pubsub_pub_task(void *pvParameter)
{
	printf("OPC UA running on Core: %i\n", xPortGetCoreID());
	while (1) {

		UA_ServerConfig *config;
		ESP_LOGI(TAG, "Fire up OPC UA Server.");
		config = UA_ServerConfig_new_default();

		/* Details about the connection configuration and handling are located in the pubsub connection tutorial */
		config->pubsubTransportLayers = (UA_PubSubTransportLayer *) UA_calloc(1, sizeof(UA_PubSubTransportLayer));
		if (!config->pubsubTransportLayers) {
			UA_ServerConfig_delete(config);
			return;
		}
		ESP_LOGI(TAG, "[APP] Free memory in Loop: %d bytes", esp_get_free_heap_size());

		config->customHostname = UA_STRING("ESP32");
		UA_String esp32url = UA_String_fromChars("opc.udp://raspberrypi:4840");
		config->applicationDescription.discoveryUrls = &esp32url;
		config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
		config->pubsubTransportLayersSize++;
		config->serverCertificate = UA_BYTESTRING("-----BEGIN CERTIFICATE-----\n"
													"MIIDfDCCAmSgAwIBAgIJAIImgnWzD+pUMA0GCSqGSIb3DQEBCwUAMFMxETAPBgNV\n"
													"BAMMCDEwLjAuMC4xMQwwCgYDVQQKDANTTUMxDDAKBgNVBAsMA0dUQzEiMCAGCSqG\n"
													"SIb3DQEJARYTbWlvc2dhLm1hcmlvQHNtYy5kZTAeFw0xOTAxMjExMDI2NTdaFw0z\n"
													"MjAxMTgxMDI2NTdaMFMxETAPBgNVBAMMCDEwLjAuMC4xMQwwCgYDVQQKDANTTUMx\n"
													"DDAKBgNVBAsMA0dUQzEiMCAGCSqGSIb3DQEJARYTbWlvc2dhLm1hcmlvQHNtYy5k\n"
													"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMAOtn/EzzX1GPpyY1Qt\n"
													"Yto9Z2Mhv4pxaTFocPom3q/wh+30dIpcWGeGXTLpMZ+uhWnowdcKwaIp8CBFFv6H\n"
													"M5+k1m2o1cybiQ1McO+B5rn6Ybm/3f+8mskz0yTGmiN4JZ5lKDWpoEov0yhNZA23\n"
													"Z733qsns2ah/wAKCbV1NlBVf7EYCItpcE7Hr2OTzID+yPtMZR4oZ0AvX365+7U/z\n"
													"2rWx0JAMWY3udA8rbDM4WYpxT3XNUKdKv9leOvFp2DRgBUkQPvKoOJhg0BrJrafN\n"
													"t7mcmeGy4wnQF6UYMMCvGqXtSGL+qZ/YeCo8e5yJe7nphGHLOvNmNVK+of6nYFMg\n"
													"ob8CAwEAAaNTMFEwHQYDVR0OBBYEFBkijOy+J5le0nF44xFpG9dahefYMB8GA1Ud\n"
													"IwQYMBaAFBkijOy+J5le0nF44xFpG9dahefYMA8GA1UdEwEB/wQFMAMBAf8wDQYJ\n"
													"KoZIhvcNAQELBQADggEBAJqVOboE69ZXSoHOffv+SMLjyUcZihkQgTCyzVQeKoxi\n"
													"0nb536PX0LvJolYPqMmhTugsGk/pBO1I8h3fOscttQAVFqGlWql2HjC9d1YWIt31\n"
													"NJnIpenf4r1SU3+Jrlp7DfVvISecG60l9HHcOkEcNvqH0PYjzZlMhPMsb1K+qXj9\n"
													"7UNJY6F1ES0hQY4JSShF/aaLJ1E6Yn2Wvc9hbkpz3Aje/Wy+QCGtb03I+vNd6pOC\n"
													"9iZuWVpUVi4E/fSfG1yo9wGbkO1yMqgcjAdLhHhZLnyGq/QeVXbRuIJvStrKhBDQ\n"
													"ouNFgdU9OG1Qb0C0fi3xp3CPK1jyO1xE2DMuEjHUm5A=\n"
													"-----END CERTIFICATE-----\n");

		UA_Server *server = UA_Server_new(config);
#ifdef SRC_DRIVER_PRSB25_H_
		ESP_ERROR_CHECK(spi_master_config());
#elif defined DRIVER_MB1222_H_
		ESP_ERROR_CHECK(i2c_master_init());
#endif
		UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent, dataSetWriterIdent;
		esp_base_mac_addr_set(mac);
		esp_efuse_mac_get_default(mac);

		addPubSubConnection(server, UA_STRING("Connection 1"), UA_STRING("opc.udp://224.0.0.22:4840/"), &connectionIdent);
		addPublishedDataSet(server, UA_STRING("Message 1"), &publishedDataSetIdent);
		addWriterGroup(server, connectionIdent, UA_STRING("WriterGroup 1"), 10, &writerGroupIdent);
		addDataSetWriter(server, writerGroupIdent, publishedDataSetIdent, UA_STRING("DataSetWriter 1"), &dataSetWriterIdent);

		addNewDataSetField(server, UA_STRING("ID"), UA_NODEID_NUMERIC(0, 6001), publishedDataSetIdent);
		addNewDataSetField(server, UA_STRING("Value"), UA_NODEID_NUMERIC(0, 6002), publishedDataSetIdent);
		addNewDataSetField(server, UA_STRING("Temperature"), UA_NODEID_NUMERIC(0, 6003), publishedDataSetIdent);

		writeMACadr(server);

		UA_Boolean running = true;
		UA_Server_run_startup(server);
		UA_Boolean waitInternal = false;
		while (running) {
			update(server);
			UA_UInt16 timeout = UA_Server_run_iterate(server, waitInternal);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = timeout * 1000;
			select(0, NULL, NULL, NULL, &tv);
		}

		ESP_LOGI(TAG, "Now going to stop the server.");
		UA_Server_delete(server);
		UA_ServerConfig_delete(config);
		ESP_LOGI(TAG, "opcua_task going to return");
		vTaskDelete(NULL);
	}
}

void addPubSubConnection(UA_Server *server, UA_String connectionName, UA_String addressUrl, UA_NodeId *assignedId)
{
	UA_PubSubConnectionConfig connectionConfig;
	memset(&connectionConfig, 0, sizeof(UA_PubSubConnectionConfig));
	connectionConfig.name = connectionName;
	connectionConfig.transportProfileUri = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
	connectionConfig.enabled = UA_TRUE;

	UA_NetworkAddressUrlDataType networkAddressUrl = {
			UA_STRING_NULL,
			addressUrl };
	UA_Variant_setScalar(&connectionConfig.address, &networkAddressUrl, &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);

	char *mac_buf = UA_malloc(sizeof(char) * 13);
	snprintf(mac_buf, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	UA_String ua_mac = UA_String_fromChars(mac_buf);

	connectionConfig.publisherId.string = ua_mac;

	UA_Server_addPubSubConnection(server, &connectionConfig, assignedId);
	UA_free(mac_buf);
	UA_String_deleteMembers(&ua_mac);

}

/**
 * **PublishedDataSet handling**
 * The PublishedDataSet (PDS) and PubSubConnection are the toplevel entities and can exist alone. The PDS contains
 * the collection of the published fields.
 * All other PubSub elements are directly or indirectly linked with the PDS or connection.
 */
void addPublishedDataSet(UA_Server *server, UA_String pdsName, UA_NodeId *assignedId)
{
	UA_PublishedDataSetConfig pdsConfig;
	memset(&pdsConfig, 0, sizeof(UA_PublishedDataSetConfig));
	pdsConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS;
	pdsConfig.name = pdsName;

	UA_Server_addPublishedDataSet(server, &pdsConfig, assignedId);
}

/**
 * **WriterGroup handling**
 * The WriterGroup (WG) is part of the connection and contains the primary configuration
 * parameters for the message creation.
 */
void addWriterGroup(UA_Server *server, UA_NodeId parentConnection, UA_String name, UA_Duration interval, UA_NodeId *assignedId)
{
	UA_WriterGroupConfig writerGroupConfig;
	memset(&writerGroupConfig, 0, sizeof(writerGroupConfig));
	writerGroupConfig.name = name;
	writerGroupConfig.publishingInterval = interval;
	writerGroupConfig.enabled = UA_FALSE;
	writerGroupConfig.writerGroupId = 100;
	writerGroupConfig.encodingMimeType = UA_PUBSUB_ENCODING_UADP;
	writerGroupConfig.securityMode = 3;

	UA_Server_addWriterGroup(server, parentConnection, &writerGroupConfig, assignedId);
}

/**
 * **DataSetWriter handling**
 * A DataSetWriter (DSW) is the glue between the WG and the PDS. The DSW is linked to exactly one
 * PDS and contains additional informations for the message generation.
 */
void addDataSetWriter(UA_Server *server, UA_NodeId parentWriterGroup, UA_NodeId connectedPDS, UA_String name, UA_NodeId *assignedId)
{
	UA_DataSetWriterConfig dataSetWriterConfig;
	memset(&dataSetWriterConfig, 0, sizeof(dataSetWriterConfig));
	dataSetWriterConfig.name = name;
	dataSetWriterConfig.keyFrameCount = 10;

	UA_Server_addDataSetWriter(server, parentWriterGroup, connectedPDS, &dataSetWriterConfig, assignedId);
}

/**
 * **DataSetField handling**
 * The DataSetField (DSF) is part of the PDS and describes exactly one published field.
 */
void addNewDataSetField(UA_Server *server, UA_String name, UA_NodeId valueNodeId, UA_NodeId publishedDataSetIdent)
{

	char* convert = (char*) UA_malloc(sizeof(char) * name.length + 1);
	memcpy(convert, name.data, name.length);
	convert[name.length] = '\0';

	UA_VariableAttributes attr = UA_VariableAttributes_default;
	attr.minimumSamplingInterval = 0.000000;
	attr.userAccessLevel = 3;
	attr.accessLevel = 3;
	attr.valueRank = -1;
	attr.writeMask = 0;
	attr.userWriteMask = 0;
	UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, valueNodeId, UA_NODEID_NUMERIC(0, UA_NS0ID_PUBLISHSUBSCRIBE), UA_NODEID_NUMERIC(0, 47),
							UA_QUALIFIEDNAME(1, convert), UA_NODEID_NUMERIC(0, 63), (const UA_NodeAttributes*) &attr,
							&UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],
							NULL,
							&valueNodeId);

	/* Add a field to the previous created PublishedDataSet */
	UA_NodeId dataSetFieldIdent;
	UA_DataSetFieldConfig dataSetFieldConfig;
	memset(&dataSetFieldConfig, 0, sizeof(UA_DataSetFieldConfig));
	dataSetFieldConfig.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
	dataSetFieldConfig.field.variable.fieldNameAlias = name;
	dataSetFieldConfig.field.variable.promotedField = UA_FALSE;
	dataSetFieldConfig.field.variable.publishParameters.publishedVariable = valueNodeId;
	dataSetFieldConfig.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;
	UA_Server_addDataSetField(server, publishedDataSetIdent, &dataSetFieldConfig, &dataSetFieldIdent);

	free(convert);
}

void removeNode(UA_Server *server, UA_NodeId nodeId)
{
	UA_Server_deleteNode(server, nodeId, true);
	UA_NodeId_deleteMembers(&nodeId);

}

void update(UA_Server *server)
{

	UA_NodeId nodeID = findNode(server, UA_QUALIFIEDNAME(1, "Temperature"), UA_NODEID_NUMERIC(0, 47),
								UA_NODEID_NUMERIC(0, UA_NS0ID_PUBLISHSUBSCRIBE));
	parseTemperature(server, nodeID);
	nodeID = findNode(server, UA_QUALIFIEDNAME(1, "Value"), UA_NODEID_NUMERIC(0, 47), UA_NODEID_NUMERIC(0, UA_NS0ID_PUBLISHSUBSCRIBE));
	parseSensorValue(server, nodeID);
}

UA_NodeId findNode(UA_Server *server_, UA_QualifiedName targetName, UA_NodeId referenceTypeId, UA_NodeId startingNode)
{
	UA_NodeId resultNodeId;
	UA_RelativePathElement rpe;
	UA_RelativePathElement_init(&rpe);
	rpe.referenceTypeId = referenceTypeId;
	rpe.isInverse = false;
	rpe.includeSubtypes = false;
	rpe.targetName = targetName;
	UA_BrowsePath bp;
	UA_BrowsePath_init(&bp);
	bp.startingNode = startingNode;
	bp.relativePath.elementsSize = 1;
	bp.relativePath.elements = &rpe;
	UA_BrowsePathResult bpr = UA_Server_translateBrowsePathToNodeIds(server_, &bp);
	if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1) {
		printf("Node not found!");
		return UA_NODEID_NULL;
	}
	UA_NodeId_copy(&bpr.targets[0].targetId.nodeId, &resultNodeId);
	UA_BrowsePathResult_deleteMembers(&bpr);
	return resultNodeId;
}

void writeMACadr(UA_Server *server)
{
	UA_NodeId nodeID = findNode(server, UA_QUALIFIEDNAME(1, "ID"), UA_NODEID_NUMERIC(0, 47), UA_NODEID_NUMERIC(0, UA_NS0ID_PUBLISHSUBSCRIBE));

	char *mac_buf = UA_malloc(sizeof(char) * 13);
	snprintf(mac_buf, 13, "%X%X%X%X%X%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	UA_String ua_mac = UA_String_fromChars(mac_buf);
	UA_Variant value;
	UA_Variant_setScalar(&value, &ua_mac, &UA_TYPES[UA_TYPES_STRING]);
	UA_Server_writeValue(server, nodeID, value);
	UA_free(mac_buf);
	UA_String_deleteMembers(&ua_mac);

}

void parseTemperature(UA_Server *server, const UA_NodeId nodeId)
{
	float temp;
	char *buf = UA_malloc(sizeof(char) * 10);

	temp = (temprature_sens_read() - 32) / 1.8;
	snprintf(buf, 10, "%f", temp);
	UA_String temperature = UA_String_fromChars(buf);
	UA_Variant value;
	UA_Variant_setScalar(&value, &temperature, &UA_TYPES[UA_TYPES_STRING]);
	UA_Server_writeValue(server, nodeId, value);
	UA_free(buf);
	UA_String_deleteMembers(&temperature);

}

void parseSensorValue(UA_Server *server, const UA_NodeId nodeId)
{

	int ret;
	char *buf = UA_malloc(sizeof(char) * 10);
#ifdef SRC_DRIVER_PRSB25_H_
	double sensor_data;
	ret = spi_master_read_sensor(&sensor_data);
	sensor_data = round(sensor_data * 1000.0) / 1000.0;
	snprintf(buf, 10, "%f", sensor_data);
#elif defined DRIVER_MB1222_H_
	uint16_t sensor_data;
	ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data);
	snprintf(buf, 10, "%i", sensor_data);
#endif
	vTaskDelay(100);

	if (ret == ESP_ERR_TIMEOUT) {
		ESP_LOGE(TAG, "Sensor Timeout");
	} else if (ret == ESP_OK) {
		UA_String sensorValue = UA_String_fromChars(buf);
		UA_Variant value;
		UA_Variant_setScalar(&value, &sensorValue, &UA_TYPES[UA_TYPES_STRING]);
		UA_Server_writeValue(server, nodeId, value);

		UA_String_deleteMembers(&sensorValue);
	} else if (ret == ESP_ERR_NOT_FOUND) {
//		ESP_LOGW(TAG, "%s: CRC check Failed ", esp_err_to_name(ret));
	} else {
//		ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
	}
	UA_free(buf);

}

