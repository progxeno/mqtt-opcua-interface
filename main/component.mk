COMPONENT_EMBED_TXTFILES := ../Mosquitto_Files/certs/ca.pem

COMPONENT_ADD_INCLUDEDIRS := . ../build/include ../src/iot/opc_ua ../src/iot/mqtt ../src/driver ../src/mbedtls ../src/mbedtls/mbedtls ../src/driver/DHT22/include

COMPONENT_SRCDIRS := $(COMPONENT_ADD_INCLUDEDIRS)