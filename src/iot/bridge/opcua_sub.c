/*
 * opcua_sub.c
 *
 *  Created on: 13.03.2019
 *      Author: miosga.mario
 */

#include "opcua_sub.h"

void opcua_sub_task(void *pvParameter)
{
	printf("OPC UA running on Core: %i\n", xPortGetCoreID());
	while (1) {
		if (xSemaphore != NULL) {
			/* See if we can obtain the semaphore.  If the semaphore is not
			 available wait 10 ticks to see if it becomes free. */
			if ( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
				UA_String transportProfile = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
				UA_NetworkAddressUrlDataType networkAddressUrl = {
						UA_STRING_NULL,
						UA_STRING("opc.udp://224.0.0.22:4840/") };
				UA_ServerConfig *config = UA_ServerConfig_new_minimal(4801, NULL);
				/* Details about the PubSubTransportLayer can be found inside the
				 * tutorial_pubsub_connection */
				config->pubsubTransportLayers = (UA_PubSubTransportLayer *) UA_calloc(2, sizeof(UA_PubSubTransportLayer));
				if (!config->pubsubTransportLayers) {
					UA_ServerConfig_delete(config);
				}
				config->customHostname = UA_STRING("ESP32");
				UA_String esp32url = UA_String_fromChars("opc.udp://raspberrypi:4840");
				config->applicationDescription.discoveryUrls = &esp32url;
				config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
				config->pubsubTransportLayersSize++;

				UA_Server *server = UA_Server_new(config);

				UA_PubSubConnectionConfig connectionConfig;
				memset(&connectionConfig, 0, sizeof(connectionConfig));
				connectionConfig.name = UA_STRING("UADP Connection 1");
				connectionConfig.transportProfileUri = transportProfile;
				connectionConfig.enabled = UA_TRUE;
				UA_Variant_setScalar(&connectionConfig.address, &networkAddressUrl, &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);
				UA_NodeId connectionIdent;
				UA_StatusCode retval = UA_Server_addPubSubConnection(server, &connectionConfig, &connectionIdent);
				if (retval == UA_STATUSCODE_GOOD)
					UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "The PubSub Connection was created successfully!");

				/* The following lines register the listening on the configured multicast
				 * address and configure a repeated job, which is used to handle received
				 * messages. */
				UA_PubSubConnection *connection = UA_PubSubConnection_findConnectionbyId(server, connectionIdent);

				if (connection != NULL) {

					UA_StatusCode rv = connection->channel->regist(connection->channel, NULL);
					if (rv == UA_STATUSCODE_GOOD) {
						UA_UInt64 subscriptionCallbackId;
						UA_Server_addRepeatedCallback(server, (UA_ServerCallback) subscriptionPollingCallback, connection, 10,
														&subscriptionCallbackId);
					} else {
						UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "register channel failed: %s!", UA_StatusCode_name(rv));
					}
				}

				UA_Boolean running = UA_TRUE;
				retval |= UA_Server_run(server, &running);
				xSemaphoreGive(xSemaphore);
				vTaskDelay(100 / portTICK_RATE_MS);

				UA_Server_delete(server);
				UA_ServerConfig_delete(config);
			} else {
				continue;
			}
		} else {
			continue;
		}
	}
}
void subscriptionPollingCallback(UA_Server *server, UA_PubSubConnection *connection)
{
	if (xSemaphore != NULL) {
		/* See if we can obtain the semaphore.  If the semaphore is not
		 available wait 10 ticks to see if it becomes free. */
		if ( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
			UA_ByteString buffer;
			if (UA_ByteString_allocBuffer(&buffer, 512) != UA_STATUSCODE_GOOD) {
				UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Message buffer allocation failed!");
				return;
			}
			UA_DateTimeStruct receivedTime = UA_DateTime_toStruct(UA_DateTime_now());
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Message content: [DateTime] \t"
											"Received date: %02i-%02i-%02i Received time: %02i:%02i:%02i",
											receivedTime.year, receivedTime.month, receivedTime.day, receivedTime.hour, receivedTime.min, receivedTime.sec);
			/* Receive the message. Blocks for 5ms */
			UA_StatusCode retval = connection->channel->receive(connection->channel, &buffer, NULL, 5);
			if (retval != UA_STATUSCODE_GOOD || buffer.length == 0) {
				/* Workaround!! Reset buffer length. Receive can set the length to zero.
				 * Then the buffer is not deleted because no memory allocation is
				 * assumed.
				 * TODO: Return an error code in 'receive' instead of setting the buf
				 * length to zero. */
				buffer.length = 512;
				UA_ByteString_clear(&buffer);
				xSemaphoreGive(xSemaphore);
				vTaskDelay(100 / portTICK_RATE_MS);

				return;
			}

			/* Decode the message */
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Message length: %lu", (unsigned long) buffer.length);
			UA_NetworkMessage networkMessage;
			memset(&networkMessage, 0, sizeof(UA_NetworkMessage));
			size_t currentPosition = 0;
			UA_NetworkMessage_decodeBinary(&buffer, &currentPosition, &networkMessage);
			UA_ByteString_clear(&buffer);

			/* Is this the correct message type? */
			if (networkMessage.networkMessageType != UA_NETWORKMESSAGE_DATASET)
				goto cleanup;

			/* At least one DataSetMessage in the NetworkMessage? */
			if (networkMessage.payloadHeaderEnabled && networkMessage.payloadHeader.dataSetPayloadHeader.count < 1)
				goto cleanup;

			/* Is this a KeyFrame-DataSetMessage? */
			UA_DataSetMessage *dsm = &networkMessage.payload.dataSetPayload.dataSetMessages[0];
			if (dsm->header.dataSetMessageType != UA_DATASETMESSAGE_DATAKEYFRAME)
				goto cleanup;

			char TxBuffer[MaxQueueSize][OPC_UA_BUF_SIZE];
			/* Loop over the fields and print well-known content types */
			for (int i = 0; i < dsm->data.keyFrameData.fieldCount; i++) {
				const UA_DataType *currentType = dsm->data.keyFrameData.dataSetFields[i].value.type;
				if (currentType == &UA_TYPES[UA_TYPES_STRING]) {
					UA_String value = *(UA_String *) dsm->data.keyFrameData.dataSetFields[i].value.data;
					UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Message content: [String] \tReceived data: \"%.*s\"", value.length,
								value.data);
					sprintf(TxBuffer[i], "%.*s", value.length, value.data);
				} else if (currentType == &UA_TYPES[UA_TYPES_DATETIME]) {
					UA_DateTime value = *(UA_DateTime *) dsm->data.keyFrameData.dataSetFields[i].value.data;
					UA_DateTimeStruct receivedTime = UA_DateTime_toStruct(value);
					UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Message content: [DateTime] \t"
								"Received date: %02i-%02i-%02i Received time: %02i:%02i:%02i",
								receivedTime.year, receivedTime.month, receivedTime.day, receivedTime.hour, receivedTime.min, receivedTime.sec);
				} else if (currentType == &UA_TYPES[UA_TYPES_BYTE]) {
					UA_Byte value = *(UA_Byte *) dsm->data.keyFrameData.dataSetFields[i].value.data;
					UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Message content: [Byte] \tReceived data: %i", value);
				}
			}


			for (int i = 0; i <= 2; i++) {
				xQueueSend(MyQueueHandleId, TxBuffer[i], (TickType_t ) 10);
				vTaskDelay(10);
			}
			xSemaphoreGive(xSemaphore);
			vTaskDelay(20);
//			if (pdTRUE == xQueueSend(MyQueueHandleId, &TxBuffer, 100)) {
//				printf("OPCUA_SUB: Successfully sent the data\n");
//				xSemaphoreGive(xSemaphore);
//			}
//			vTaskDelay(100);
			cleanup: UA_NetworkMessage_deleteMembers(&networkMessage);
		}
	}
}
