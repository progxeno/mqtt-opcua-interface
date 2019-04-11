Add:

#define UA_ENABLE_PUBSUB

#ifdef UA_ENABLE_PUBSUB /* conditional compilation */

	/*
	 * List definitions.
	 */
#define LIST_HEAD(name, type)						\
	struct name {								\
	    struct type *lh_first;	/* first element */			\
	}

#define LIST_HEAD_INITIALIZER(head)					\
	    { NULL }

#define LIST_ENTRY(type)						\
	struct {								\
	    struct type *le_next;	/* next element */			\
	    struct type **le_prev;	/* address of previous next element */	\
	}

	/*
	 * List access methods
	 */
#define	LIST_FIRST(head)		((head)->lh_first)
#define	LIST_END(head)			NULL
#define	LIST_NEXT(elm, field)		((elm)->field.le_next)

#define LIST_REPLACE(elm, elm2, field) do {				\
	    if (((elm2)->field.le_next = (elm)->field.le_next) != NULL)	\
	        (elm2)->field.le_next->field.le_prev =			\
	            &(elm2)->field.le_next;				\
	    (elm2)->field.le_prev = (elm)->field.le_prev;			\
	    *(elm2)->field.le_prev = (elm2);				\
	    _Q_INVALIDATE((elm)->field.le_prev);				\
	    _Q_INVALIDATE((elm)->field.le_next);				\
	} while (0)

	/* DataSet Payload Header */
	typedef struct {
	    UA_Byte count;
	    UA_UInt16* dataSetWriterIds;
	} UA_DataSetPayloadHeader;

	/* FieldEncoding Enum  */
	typedef enum {
	    UA_FIELDENCODING_VARIANT = 0,
	    UA_FIELDENCODING_RAWDATA = 1,
	    UA_FIELDENCODING_DATAVALUE = 2
	} UA_FieldEncoding;

	/* DataSetMessage Type */
	typedef enum {
	    UA_DATASETMESSAGE_DATAKEYFRAME = 0,
	    UA_DATASETMESSAGE_DATADELTAFRAME = 1,
	    UA_DATASETMESSAGE_EVENT = 2,
	    UA_DATASETMESSAGE_KEEPALIVE = 3
	} UA_DataSetMessageType;

	/* DataSetMessage Header */
	typedef struct {
	    UA_Boolean dataSetMessageValid;
	    UA_FieldEncoding fieldEncoding;
	    UA_Boolean dataSetMessageSequenceNrEnabled;
	    UA_Boolean timestampEnabled;
	    UA_Boolean statusEnabled;
	    UA_Boolean configVersionMajorVersionEnabled;
	    UA_Boolean configVersionMinorVersionEnabled;
	    UA_DataSetMessageType dataSetMessageType;
	    UA_Boolean picoSecondsIncluded;
	    UA_UInt16 dataSetMessageSequenceNr;
	    UA_UtcTime timestamp;
	    UA_UInt16 picoSeconds;
	    UA_UInt16 status;
	    UA_UInt32 configVersionMajorVersion;
	    UA_UInt32 configVersionMinorVersion;
	} UA_DataSetMessageHeader;

	UA_StatusCode
	UA_DataSetMessageHeader_encodeBinary(const UA_DataSetMessageHeader* src,
	                                     UA_Byte **bufPos, const UA_Byte *bufEnd);

	UA_StatusCode
	UA_DataSetMessageHeader_decodeBinary(const UA_ByteString *src, size_t *offset,
	                                     UA_DataSetMessageHeader* dst);

	size_t
	UA_DataSetMessageHeader_calcSizeBinary(const UA_DataSetMessageHeader* p);

	/**
	 * DataSetMessage
	 * ^^^^^^^^^^^^^^ */

	typedef struct {
	    UA_UInt16 fieldCount;
	    UA_DataValue* dataSetFields;
	} UA_DataSetMessage_DataKeyFrameData;

	typedef struct {
	    UA_UInt16 fieldIndex;
	    UA_DataValue fieldValue;
	} UA_DataSetMessage_DeltaFrameField;

	typedef struct {
	    UA_UInt16 fieldCount;
	    UA_DataSetMessage_DeltaFrameField* deltaFrameFields;
	} UA_DataSetMessage_DataDeltaFrameData;

	typedef struct {
	    UA_DataSetMessageHeader header;
	    union {
	        UA_DataSetMessage_DataKeyFrameData keyFrameData;
	        UA_DataSetMessage_DataDeltaFrameData deltaFrameData;
	    } data;
	} UA_DataSetMessage;

	UA_StatusCode
	UA_DataSetMessage_encodeBinary(const UA_DataSetMessage* src, UA_Byte **bufPos,
	                               const UA_Byte *bufEnd);

	UA_StatusCode
	UA_DataSetMessage_decodeBinary(const UA_ByteString *src, size_t *offset,
	                               UA_DataSetMessage* dst);

	size_t
	UA_DataSetMessage_calcSizeBinary(const UA_DataSetMessage* p);

	void UA_DataSetMessage_free(const UA_DataSetMessage* p);

	typedef struct {
	    UA_UInt16* sizes;
	    UA_DataSetMessage* dataSetMessages;
	} UA_DataSetPayload;

	typedef enum {
	    UA_PUBLISHERDATATYPE_BYTE = 0,
	    UA_PUBLISHERDATATYPE_UINT16 = 1,
	    UA_PUBLISHERDATATYPE_UINT32 = 2,
	    UA_PUBLISHERDATATYPE_UINT64 = 3,
	    UA_PUBLISHERDATATYPE_STRING = 4
	} UA_PublisherIdDatatype;

	typedef enum {
	    UA_NETWORKMESSAGE_DATASET = 0,
	    UA_NETWORKMESSAGE_DISCOVERY_REQUEST = 1,
	    UA_NETWORKMESSAGE_DISCOVERY_RESPONSE = 2
	} UA_NetworkMessageType;

	/**
	 * UA_NetworkMessageGroupHeader
	 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
	typedef struct {
	    UA_Boolean writerGroupIdEnabled;
	    UA_Boolean groupVersionEnabled;
	    UA_Boolean networkMessageNumberEnabled;
	    UA_Boolean sequenceNumberEnabled;
	    UA_UInt16 writerGroupId;
	    UA_UInt32 groupVersion; // spec: type "VersionTime"
	    UA_UInt16 networkMessageNumber;
	    UA_UInt16 sequenceNumber;
	} UA_NetworkMessageGroupHeader;

	/**
	 * UA_NetworkMessageSecurityHeader
	 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
	typedef struct {
	    UA_Boolean networkMessageSigned;
	    UA_Boolean networkMessageEncrypted;
	    UA_Boolean securityFooterEnabled;
	    UA_Boolean forceKeyReset;
	    UA_UInt32 securityTokenId;      // spec: IntegerId
	    UA_Byte nonceLength;
	    UA_ByteString messageNonce;
	    UA_UInt16 securityFooterSize;
	} UA_NetworkMessageSecurityHeader;

	/**
	 * UA_NetworkMessage
	 * ^^^^^^^^^^^^^^^^^ */
	typedef struct {
	    UA_Byte version;
	    UA_Boolean publisherIdEnabled;
	    UA_Boolean groupHeaderEnabled;
	    UA_Boolean payloadHeaderEnabled;
	    UA_PublisherIdDatatype publisherIdType;
	    UA_Boolean dataSetClassIdEnabled;
	    UA_Boolean securityEnabled;
	    UA_Boolean timestampEnabled;
	    UA_Boolean picosecondsEnabled;
	    UA_Boolean chunkMessage;
	    UA_Boolean promotedFieldsEnabled;
	    UA_NetworkMessageType networkMessageType;
	    union {
	        UA_Byte publisherIdByte;
	        UA_UInt16 publisherIdUInt16;
	        UA_UInt32 publisherIdUInt32;
	        UA_UInt64 publisherIdUInt64;
	        UA_Guid publisherIdGuid;
	        UA_String publisherIdString;
	    } publisherId;
	    UA_Guid dataSetClassId;

	    UA_NetworkMessageGroupHeader groupHeader;

	    union {
	        UA_DataSetPayloadHeader dataSetPayloadHeader;
	    } payloadHeader;

	    UA_DateTime timestamp;
	    UA_UInt16 picoseconds;
	    UA_UInt16 promotedFieldsSize;
	    UA_Variant* promotedFields;	/* BaseDataType */

	    UA_NetworkMessageSecurityHeader securityHeader;

	    union {
	        UA_DataSetPayload dataSetPayload;
	    } payload;

	    UA_ByteString securityFooter;
	    UA_ByteString signature;
	} UA_NetworkMessage;

	UA_StatusCode
	UA_NetworkMessage_encodeBinary(const UA_NetworkMessage* src,
	                               UA_Byte **bufPos, const UA_Byte *bufEnd);

	UA_StatusCode
	UA_NetworkMessage_decodeBinary(const UA_ByteString *src, size_t *offset,
	                               UA_NetworkMessage* dst);

	size_t
	UA_NetworkMessage_calcSizeBinary(const UA_NetworkMessage* p);

	void
	UA_NetworkMessage_deleteMembers(UA_NetworkMessage* p);

	void
	UA_NetworkMessage_delete(UA_NetworkMessage* p);

//forward declarations
	struct UA_WriterGroup;
	typedef struct UA_WriterGroup UA_WriterGroup;

	/* The configuration structs (public part of PubSub entities) are defined in include/ua_plugin_pubsub.h */

	/**********************************************/
	/*            PublishedDataSet                */
	/**********************************************/
	typedef struct
	{
			UA_PublishedDataSetConfig config;
			UA_DataSetMetaDataType dataSetMetaData;LIST_HEAD(UA_ListOfDataSetField, UA_DataSetField)
				fields;
				UA_NodeId identifier;
				UA_UInt16 fieldSize;
				UA_UInt16 promotedFieldsCount;
		} UA_PublishedDataSet;

		UA_StatusCode
		UA_PublishedDataSetConfig_copy(const UA_PublishedDataSetConfig *src, UA_PublishedDataSetConfig *dst);
		UA_PublishedDataSet *
		UA_PublishedDataSet_findPDSbyId(UA_Server *server, UA_NodeId identifier);
		void
		UA_PublishedDataSet_deleteMembers(UA_Server *server, UA_PublishedDataSet *publishedDataSet);

		/**********************************************/
		/*               Connection                   */
		/**********************************************/
//the connection config (public part of connection) object is defined in include/ua_plugin_pubsub.h
		typedef struct
		{
				UA_PubSubConnectionConfig *config;
				//internal fields
				UA_PubSubChannel *channel;
				UA_NodeId identifier;LIST_HEAD(UA_ListOfWriterGroup, UA_WriterGroup)
					writerGroups;
			} UA_PubSubConnection;

			UA_StatusCode
			UA_PubSubConnectionConfig_copy(const UA_PubSubConnectionConfig *src, UA_PubSubConnectionConfig *dst);
			UA_PubSubConnection *
			UA_PubSubConnection_findConnectionbyId(UA_Server *server, UA_NodeId connectionIdentifier);
			void
			UA_PubSubConnectionConfig_deleteMembers(UA_PubSubConnectionConfig *connectionConfig);
			void
			UA_PubSubConnection_deleteMembers(UA_Server *server, UA_PubSubConnection *connection);

			/**********************************************/
			/*              DataSetWriter                 */
			/**********************************************/

#ifdef UA_ENABLE_PUBSUB_DELTAFRAMES
			typedef struct UA_DataSetWriterSample {
				UA_Boolean valueChanged;
				UA_DataValue value;
			}UA_DataSetWriterSample;
#endif

			typedef struct UA_DataSetWriter
			{
					UA_DataSetWriterConfig config;
					//internal fields
					LIST_ENTRY(UA_DataSetWriter)
					listEntry;
					UA_NodeId identifier;
					UA_NodeId linkedWriterGroup;
					UA_NodeId connectedDataSet;
					UA_ConfigurationVersionDataType connectedDataSetVersion;
#ifdef UA_ENABLE_PUBSUB_DELTAFRAMES
					UA_UInt16 deltaFrameCounter;            //actual count of sent deltaFrames
					size_t lastSamplesCount;
					UA_DataSetWriterSample *lastSamples;
#endif
					UA_UInt16 actualDataSetMessageSequenceCount;
			} UA_DataSetWriter;

			UA_StatusCode
			UA_DataSetWriterConfig_copy(const UA_DataSetWriterConfig *src, UA_DataSetWriterConfig *dst);
			UA_DataSetWriter *
			UA_DataSetWriter_findDSWbyId(UA_Server *server, UA_NodeId identifier);
			void
			UA_DataSetWriter_deleteMembers(UA_Server *server, UA_DataSetWriter *dataSetWriter);

			/**********************************************/
			/*               WriterGroup                  */
			/**********************************************/

			struct UA_WriterGroup
			{
					UA_WriterGroupConfig config;
					//internal fields
					LIST_ENTRY(UA_WriterGroup)
					listEntry;
					UA_NodeId identifier;
					UA_NodeId linkedConnection;LIST_HEAD(UA_ListOfDataSetWriter, UA_DataSetWriter)
						writers;
						UA_UInt32 writersCount;
						UA_UInt64 publishCallbackId;
						UA_Boolean publishCallbackIsRegistered;
				};

				UA_StatusCode
				UA_WriterGroupConfig_copy(const UA_WriterGroupConfig *src, UA_WriterGroupConfig *dst);
				UA_WriterGroup *
				UA_WriterGroup_findWGbyId(UA_Server *server, UA_NodeId identifier);
				void
				UA_WriterGroup_deleteMembers(UA_Server *server, UA_WriterGroup *writerGroup);

				/**********************************************/
				/*               DataSetField                 */
				/**********************************************/

				typedef struct UA_DataSetField
				{
						UA_DataSetFieldConfig config;
						//internal fields
						LIST_ENTRY(UA_DataSetField)
						listEntry;
						UA_NodeId identifier;
						UA_NodeId publishedDataSet;             //ref to parent pds
						UA_FieldMetaData fieldMetaData;
						UA_UInt64 sampleCallbackId;
						UA_Boolean sampleCallbackIsRegistered;
				} UA_DataSetField;

				UA_StatusCode
				UA_DataSetFieldConfig_copy(const UA_DataSetFieldConfig *src, UA_DataSetFieldConfig *dst);
				UA_DataSetField *
				UA_DataSetField_findDSFbyId(UA_Server *server, UA_NodeId identifier);
				void
				UA_DataSetField_deleteMembers(UA_DataSetField *field);

				/*********************************************************/
				/*               PublishValues handling                  */
				/*********************************************************/

				UA_StatusCode
				UA_WriterGroup_addPublishCallback(UA_Server *server, UA_WriterGroup *writerGroup);
				void
				UA_WriterGroup_publishCallback(UA_Server *server, UA_WriterGroup *writerGroup);

#endif /* UA_ENABLE_PUBSUB */

From the open62541.c file to your own header file.

In open62541.h:
Change:
static UA_INLINE void
UA_ByteString_deleteMembers(UA_ByteString *p) {
    UA_deleteMembers(p, &UA_TYPES[UA_TYPES_BYTESTRING]);
}

To:

static UA_INLINE void
UA_ByteString_deleteMembers(UA_ByteString *p) {
    UA_deleteMembers(p, &UA_TYPES[UA_TYPES_BYTESTRING]);
}

static UA_INLINE void
UA_ByteString_clear(UA_ByteString *p) {
    UA_clear(p, &UA_TYPES[UA_TYPES_BYTESTRING]);
}


Change:
void UA_EXPORT UA_deleteMembers(void *p, const UA_DataType *type);

To:
void UA_EXPORT UA_clear(void *p, const UA_DataType *type);
#define UA_deleteMembers(p, type) UA_clear(p, type)