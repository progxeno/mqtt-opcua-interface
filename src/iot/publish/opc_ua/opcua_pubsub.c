/*
 * opcua_pubsub.c
 *
 *  Created on: 09.01.2019
 *      Author: miosga.mario
 */

#include "opcua_pubsub.h"

void opcua_pubsub_task(void *pvParameter)
{
	printf("OPC UA running on Core: %i\n", xPortGetCoreID());
	while (1) {
		if (xSemaphore != NULL) {
			/* See if we can obtain the semaphore.  If the semaphore is not
			 available wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
				vTaskDelay(10 / portTICK_RATE_MS);

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

				UA_Server *server = UA_Server_new(config);

				esp_base_mac_addr_set(mac);
				esp_efuse_mac_get_default(mac);
				addPubSubConnection(server, UA_STRING("Connection 1"), UA_STRING("opc.udp://224.0.0.22:4840/"), &connectionIdent);
				addPublishedDataSet(server, UA_STRING("Message 1"), &publishedDataSetIdent);
				addWriterGroup(server, connectionIdent, UA_STRING("WriterGroup 1"), 10, &writerGroupIdent);
				addDataSetWriter(server, writerGroupIdent, publishedDataSetIdent, UA_STRING("DataSetWriter 1"), &dataSetWriterIdent);
//				addDataSetField(server);

				addNewDataSetField(server, UA_STRING("ID"), UA_NODEID_NUMERIC(0, 6001));
				addNewDataSetField(server, UA_STRING("Value"), UA_NODEID_NUMERIC(0, 6002));
				addNewDataSetField(server, UA_STRING("Temperature"), UA_NODEID_NUMERIC(0, 6003));

				UA_Server_run_startup(server);

				UA_Boolean waitInternal = false;
				xSemaphoreGive(xSemaphore);
				vTaskDelay(500 / portTICK_RATE_MS);
				while (running) {
					char RxBuffer[1][OPC_UA_BUF_SIZE];
					/* See if we can obtain the semaphore.  If the semaphore is not
					 available wait 10 ticks to see if it becomes free. */
					if (pdTRUE == xQueueReceive(MyQueueHandleId, RxBuffer[0], 10) && xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
//						parseTemperature(server, createdNodeId, &RxBuffer[0]);
//						browsing(server, createdNodeId);
						update(server, UA_QUALIFIEDNAME(0, "ID"), &RxBuffer[0]);
//						update(server, UA_QUALIFIEDNAME(0, "Value"),&RxBuffer[0]);
//						update(server, UA_QUALIFIEDNAME(0, "Temperature"),&RxBuffer[0]);

						UA_UInt16 timeout = UA_Server_run_iterate(server, waitInternal);
						struct timeval tv;
						tv.tv_sec = 0;
						tv.tv_usec = timeout * 1000;
						select(0, NULL, NULL, NULL, &tv);
						xSemaphoreGive(xSemaphore);
						vTaskDelay(10);
					} else {
						continue;
					}
				}

				ESP_LOGI(TAG, "Now going to stop the server.");
				UA_Server_delete(server);
				UA_ServerConfig_delete(config);
				ESP_LOGI(TAG, "opcua_task going to return");
				vTaskDelete(NULL);
			} else {
				continue;
			}
		} else {
			continue;
		}
	}
}

void addPubSubConnection(UA_Server *server, UA_String connectionName, UA_String addressUrl, UA_NodeId *assignedId)
{
	UA_PubSubConnectionConfig connectionConfig;
	memset(&connectionConfig, 0, sizeof(UA_PubSubConnectionConfig));
	connectionConfig.name = connectionName;
	UA_NetworkAddressUrlDataType networkAddressUrl = {
			UA_STRING_NULL,
			addressUrl };
	UA_Variant_setScalar(&connectionConfig.address, &networkAddressUrl, &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
	connectionConfig.transportProfileUri = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
	UA_Server_addPubSubConnection(server, &connectionConfig, assignedId);
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
 * **DataSetField handling**
 * The DataSetField (DSF) is part of the PDS and describes exactly one published field.
 */
void addDataSetField(UA_Server *server)
{
	if (myNS(server) != UA_STATUSCODE_GOOD) {
		UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Could not add the example nodeset. "
						"Check previous output for any error.");
	} else {

		UA_ObjectAttributes object_attr = UA_ObjectAttributes_default;

		object_attr.description = UA_LOCALIZEDTEXT("en-US", "A Message!");
		object_attr.displayName = UA_LOCALIZEDTEXT("en-US", "Message1");

		// we assume that the myNS nodeset was added in namespace 2.
		// You should always use UA_Server_addNamespace to check what the
		// namespace index is for a given namespace URI. UA_Server_addNamespace
		// will just return the index if it is already added.
		UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 0), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
								UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "Message1"), UA_NODEID_NUMERIC(2, 1001), object_attr,
								NULL,
								&createdNodeId);

	}
//	/* Add a field to the previous created PublishedDataSet */
//	UA_NodeId dataSetFieldIdent;
//	UA_DataSetFieldConfig dataSetFieldTemp;
//	UA_VariableAttributes attr = UA_VariableAttributes_default;
//	UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, UA_NODEID_NUMERIC(1, 6001),
//														UA_NODEID_NUMERIC(0, UA_NS0ID_PUBLISHSUBSCRIBE), UA_NODEID_NUMERIC(0, 47),
//														UA_QUALIFIEDNAME(1, "Test"), UA_NODEID_NUMERIC(0, 63), (const UA_NodeAttributes*) &attr,
//														&UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES],
//														NULL,
//														&createdNodeId);
//
//	memset(&dataSetFieldTemp, 0, sizeof(UA_DataSetFieldConfig));
//	dataSetFieldTemp.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
//	dataSetFieldTemp.field.variable.fieldNameAlias = UA_STRING("Server localtime");
//	dataSetFieldTemp.field.variable.promotedField = UA_FALSE;
//	dataSetFieldTemp.field.variable.publishParameters.publishedVariable = createdNodeId;
//	dataSetFieldTemp.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;
//
//	UA_Server_addDataSetField(server, publishedDataSetIdent, &dataSetFieldTemp, &dataSetFieldIdent);
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
	writerGroupConfig.encodingMimeType = UA_PUBSUB_ENCODING_UADP;
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
	UA_Server_addDataSetWriter(server, parentWriterGroup, connectedPDS, &dataSetWriterConfig, assignedId);
}

void removeNode(UA_Server *server, UA_NodeId nodeId)
{
	UA_Server_deleteNode(server, nodeId, true);
	UA_NodeId_deleteMembers(&nodeId);

}

void parseTemperature(UA_Server *server, const UA_NodeId nodeId, char *data)
{
//	printf("OPCUA_PUB: Successfully recieved the data %s\n", data);
	UA_String temperature = UA_String_fromChars(data);
//	cJSON * root = cJSON_Parse(data);
//		UA_String temperature = UA_String_fromChars(cJSON_GetArrayItem(root, 0)->valuestring);
//	int x = cJSON_GetArraySize(root);
	vTaskDelay(10);
//	printf("count: %i", x);
	UA_Variant value;
	UA_Variant_setScalar(&value, &temperature, &UA_TYPES[UA_TYPES_STRING]);
	vTaskDelay(10);
	UA_Server_writeValue(server, nodeId, value);
	vTaskDelay(10);
	UA_String_deleteMembers(&temperature);
//	cJSON_Delete(root);
}

void browsing(UA_Server *server, UA_NodeId nodeId)
{
	UA_UInt16 ns[2];
	ns[0] = UA_Server_addNamespace(server, "http://opcfoundation.org/UA/");
	ns[1] = UA_Server_addNamespace(server, "http://yourorganisation.org/example_nodeset/");

	UA_NodeId myIDNodeId = UA_NODEID_NUMERIC(ns[1], 6001);
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "nodeid: %d", myIDNodeId.identifier.numeric);
	UA_String ID = UA_String_fromChars("IDtest123");
	UA_Variant myVar1;
	UA_Variant_init(&myVar1);
	UA_Variant_setScalar(&myVar1, &ID, &UA_TYPES[UA_TYPES_STRING]);
	UA_StatusCode result = UA_Server_writeValue(server, myIDNodeId, myVar1);
	if (result != UA_STATUSCODE_GOOD) {
		UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Could not write value: %8x", result);
	} else {
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "node value %.*s was written", ID.length, ID.data);
	}

	UA_NodeId myValueNodeId = UA_NODEID_NUMERIC(ns[1], 6002);
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "nodeid: %d", myValueNodeId.identifier.numeric);
	UA_String value = UA_String_fromChars("126.89");
	UA_Variant myVar2;
	UA_Variant_init(&myVar2);
	UA_Variant_setScalar(&myVar2, &value, &UA_TYPES[UA_TYPES_STRING]);
	result = UA_Server_writeValue(server, myValueNodeId, myVar2);
	if (result != UA_STATUSCODE_GOOD) {
		UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Could not write value: %8x", result);
	} else {
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "node value %.*s was written", value.length, value.data);
	}

	UA_NodeId myTemperatureNodeId = UA_NODEID_NUMERIC(ns[1], 6003);
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "nodeid: %d", myTemperatureNodeId.identifier.numeric);
	UA_String temperature = UA_String_fromChars("43.8");
	UA_Variant myVar3;
	UA_Variant_init(&myVar3);
	UA_Variant_setScalar(&myVar3, &temperature, &UA_TYPES[UA_TYPES_STRING]);
	result = UA_Server_writeValue(server, myTemperatureNodeId, myVar3);
	if (result != UA_STATUSCODE_GOOD) {
		UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Could not write value: %8x", result);
	} else {
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "node value %.*s was written", temperature.length, temperature.data);
	}
	UA_String_deleteMembers(&ID);
	UA_String_deleteMembers(&value);
	UA_String_deleteMembers(&temperature);
}

void update(UA_Server *server, UA_QualifiedName targetName, char *data)
{

	UA_NodeId nodeID = findSingleChildNode(server, targetName, UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), writerGroupIdent);

	UA_String temperature = UA_String_fromChars(data);
	vTaskDelay(10);
	UA_Variant value;
	UA_Variant_setScalar(&value, &temperature, &UA_TYPES[UA_TYPES_STRING]);
	vTaskDelay(10);
	UA_Server_writeValue(server, nodeID, value);
	vTaskDelay(10);
	UA_String_deleteMembers(&temperature);

}

UA_NodeId findSingleChildNode(UA_Server *server_, UA_QualifiedName targetName, UA_NodeId referenceTypeId, UA_NodeId startingNode)
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
	if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1)
		return UA_NODEID_NULL;
	UA_NodeId_copy(&bpr.targets[0].targetId.nodeId, &resultNodeId);
	UA_BrowsePathResult_deleteMembers(&bpr);
	return resultNodeId;
}

void addNewDataSetField(UA_Server *server, UA_String name, const UA_NodeId valueNodeId)
{
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
}
