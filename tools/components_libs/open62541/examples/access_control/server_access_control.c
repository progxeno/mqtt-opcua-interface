/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

#include <ua_server.h>
#include <ua_config_default.h>
#include <ua_plugin_access_control.h>
#include <ua_accesscontrol_default.h>
#include <ua_log_stdout.h>

#include <signal.h>
#include <stdlib.h>

static UA_Boolean
allowAddNode(UA_Server *server, UA_AccessControl *ac,
             const UA_NodeId *sessionId, void *sessionContext,
             const UA_AddNodesItem *item) {
    printf("Called allowAddNode\n");
    return UA_TRUE;
}

static UA_Boolean
allowAddReference(UA_Server *server, UA_AccessControl *ac,
                  const UA_NodeId *sessionId, void *sessionContext,
                  const UA_AddReferencesItem *item) {
    printf("Called allowAddReference\n");
    return UA_TRUE;
}

static UA_Boolean
allowDeleteNode(UA_Server *server, UA_AccessControl *ac,
                const UA_NodeId *sessionId, void *sessionContext,
                const UA_DeleteNodesItem *item) {
    printf("Called allowDeleteNode\n");
    return UA_FALSE; // Do not allow deletion from client
}

static UA_Boolean
allowDeleteReference(UA_Server *server, UA_AccessControl *ac,
                     const UA_NodeId *sessionId, void *sessionContext,
                     const UA_DeleteReferencesItem *item) {
    printf("Called allowDeleteReference\n");
    return UA_TRUE;
}

UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

static UA_UsernamePasswordLogin logins[2] = {
    {UA_STRING_STATIC("peter"), UA_STRING_STATIC("peter123")},
    {UA_STRING_STATIC("paula"), UA_STRING_STATIC("paula123")}
};

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_ServerConfig *config = UA_ServerConfig_new_default();
    // disable anonymous logins, enable two user/password logins
    config->accessControl.deleteMembers(&config->accessControl);
    if (UA_AccessControl_default(&config->accessControl, false, 2, logins) != UA_STATUSCODE_GOOD) {
        return EXIT_FAILURE;
    }

    // Set accessControl functions for nodeManagement
    config->accessControl.allowAddNode = allowAddNode;
    config->accessControl.allowAddReference = allowAddReference;
    config->accessControl.allowDeleteNode = allowDeleteNode;
    config->accessControl.allowDeleteReference = allowDeleteReference;

    UA_Server *server = UA_Server_new(config);

    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
