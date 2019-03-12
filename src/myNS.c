/* WARNING: This is a generated file.
 * Any manual changes will be overwritten. */

#include "myNS.h"

static UA_StatusCode function_myNS_1_begin(UA_Server *server, UA_UInt16* ns)
{
	UA_StatusCode retVal = UA_STATUSCODE_GOOD;
	UA_ObjectTypeAttributes attr = UA_ObjectTypeAttributes_default;
	attr.isAbstract = true;
	attr.displayName = UA_LOCALIZEDTEXT("", "Message");
#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
	attr.description = UA_LOCALIZEDTEXT("", "");
#endif
	attr.writeMask = 0;
	attr.userWriteMask = 0;
	retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_OBJECTTYPE, UA_NODEID_NUMERIC(ns[1], UA_NS0ID_PUBLISHSUBSCRIBE), UA_NODEID_NUMERIC(ns[0], 58),
										UA_NODEID_NUMERIC(ns[0], 45), UA_QUALIFIEDNAME(ns[1], "Message"), UA_NODEID_NULL,
										(const UA_NodeAttributes*) &attr, &UA_TYPES[UA_TYPES_OBJECTTYPEATTRIBUTES], NULL, NULL);
	return retVal;
}

static UA_StatusCode function_myNS_1_finish(UA_Server *server, UA_UInt16* ns)
{
	return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(ns[1], UA_NS0ID_PUBLISHSUBSCRIBE));
}

/* ManufacturerName - ns=1;i=6001 */

static UA_StatusCode function_myNS_2_begin(UA_Server *server, UA_UInt16* ns)
{
	UA_StatusCode retVal = UA_STATUSCODE_GOOD;
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	attr.minimumSamplingInterval = 0.000000;
	attr.userAccessLevel = 3;
	attr.accessLevel = 3;
	attr.valueRank = -1;
	attr.dataType = UA_NODEID_NUMERIC(ns[0], 12);
	UA_STACKARRAY(UA_String, variablenode_ns_1_i_6001_variant_DataContents, 1);
	UA_init(variablenode_ns_1_i_6001_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
	UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6001_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
	attr.displayName = UA_LOCALIZEDTEXT("", "ID");
#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
	attr.description = UA_LOCALIZEDTEXT("", "");
#endif
	attr.writeMask = 0;
	attr.userWriteMask = 0;
	retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, UA_NODEID_NUMERIC(ns[1], 6001), UA_NODEID_NUMERIC(ns[1], UA_NS0ID_PUBLISHSUBSCRIBE),
										UA_NODEID_NUMERIC(ns[0], 47), UA_QUALIFIEDNAME(ns[1], "ID"), UA_NODEID_NUMERIC(ns[0], 63),
										(const UA_NodeAttributes*) &attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES], NULL, NULL);
	retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6001), UA_NODEID_NUMERIC(ns[0], 37), UA_EXPANDEDNODEID_NUMERIC(ns[0], 78),
	true);
	return retVal;
}

static UA_StatusCode function_myNS_2_finish(UA_Server *server, UA_UInt16* ns)
{
	return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(ns[1], 6001));
}

/* ModelName - ns=1;i=6002 */

static UA_StatusCode function_myNS_3_begin(UA_Server *server, UA_UInt16* ns)
{
	UA_StatusCode retVal = UA_STATUSCODE_GOOD;
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	attr.minimumSamplingInterval = 0.000000;
	attr.userAccessLevel = 3;
	attr.accessLevel = 3;
	attr.valueRank = -1;
	attr.dataType = UA_NODEID_NUMERIC(ns[0], 12);
	UA_STACKARRAY(UA_String, variablenode_ns_1_i_6002_variant_DataContents, 1);
	UA_init(variablenode_ns_1_i_6002_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
	UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6002_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
	attr.displayName = UA_LOCALIZEDTEXT("", "Value");
#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
	attr.description = UA_LOCALIZEDTEXT("", "");
#endif
	attr.writeMask = 0;
	attr.userWriteMask = 0;
	retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, UA_NODEID_NUMERIC(ns[1], 6002), UA_NODEID_NUMERIC(ns[1], UA_NS0ID_PUBLISHSUBSCRIBE),
										UA_NODEID_NUMERIC(ns[0], 47), UA_QUALIFIEDNAME(ns[1], "Value"), UA_NODEID_NUMERIC(ns[0], 63),
										(const UA_NodeAttributes*) &attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES], NULL, NULL);
	retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6002), UA_NODEID_NUMERIC(ns[0], 37), UA_EXPANDEDNODEID_NUMERIC(ns[0], 78),
	true);
	return retVal;
}

static UA_StatusCode function_myNS_3_finish(UA_Server *server, UA_UInt16* ns)
{
	return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(ns[1], 6002));
}

static UA_StatusCode function_myNS_4_begin(UA_Server *server, UA_UInt16* ns)
{
	UA_StatusCode retVal = UA_STATUSCODE_GOOD;
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	attr.minimumSamplingInterval = 0.000000;
	attr.userAccessLevel = 3;
	attr.accessLevel = 3;
	attr.valueRank = -1;
	attr.dataType = UA_NODEID_NUMERIC(ns[0], 12);
	UA_STACKARRAY(UA_String, variablenode_ns_1_i_6003_variant_DataContents, 1);
	UA_init(variablenode_ns_1_i_6003_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
	UA_Variant_setScalar(&attr.value, variablenode_ns_1_i_6003_variant_DataContents, &UA_TYPES[UA_TYPES_STRING]);
	attr.displayName = UA_LOCALIZEDTEXT("", "Temperature");
#ifdef UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
	attr.description = UA_LOCALIZEDTEXT("", "");
#endif
	attr.writeMask = 0;
	attr.userWriteMask = 0;
	retVal |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, UA_NODEID_NUMERIC(ns[1], 6003), UA_NODEID_NUMERIC(ns[1], UA_NS0ID_PUBLISHSUBSCRIBE),
										UA_NODEID_NUMERIC(ns[0], 47), UA_QUALIFIEDNAME(ns[1], "Temperature"), UA_NODEID_NUMERIC(ns[0], 63),
										(const UA_NodeAttributes*) &attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES], NULL, NULL);
	retVal |= UA_Server_addReference(server, UA_NODEID_NUMERIC(ns[1], 6003), UA_NODEID_NUMERIC(ns[0], 37), UA_EXPANDEDNODEID_NUMERIC(ns[0], 78),
	true);
	return retVal;
}

static UA_StatusCode function_myNS_4_finish(UA_Server *server, UA_UInt16* ns)
{
	return UA_Server_addNode_finish(server, UA_NODEID_NUMERIC(ns[1], 6003));
}

UA_StatusCode myNS(UA_Server *server)
{
	UA_StatusCode retVal = UA_STATUSCODE_GOOD;
	/* Use namespace ids generated by the server */
	UA_UInt16 ns[2];
	ns[0] = UA_Server_addNamespace(server, "http://opcfoundation.org/UA/");
	ns[1] = UA_Server_addNamespace(server, "http://yourorganisation.org/example_nodeset/");
	retVal |= function_myNS_1_begin(server, ns);
	retVal |= function_myNS_2_begin(server, ns);
	retVal |= function_myNS_3_begin(server, ns);
	retVal |= function_myNS_4_begin(server, ns);
	retVal |= function_myNS_4_finish(server, ns);
	retVal |= function_myNS_3_finish(server, ns);
	retVal |= function_myNS_2_finish(server, ns);
	retVal |= function_myNS_1_finish(server, ns);
	return retVal;
}
