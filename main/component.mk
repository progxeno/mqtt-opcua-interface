COMPONENT_EMBED_TXTFILES := ca.pem

COMPONENT_ADD_INCLUDEDIRS := . ../build/include ../src ../src/iot/publish/opc_ua ../src/iot/publish/mqtt ../src/iot/subscribe/opc_ua ../src/iot/subscribe/mqtt ../src/driver ../src/mbedtls ../src/mbedtls/mbedtls

COMPONENT_SRCDIRS := $(COMPONENT_ADD_INCLUDEDIRS)