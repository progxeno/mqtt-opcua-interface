deps_config := \
	/home/Labor.GTC/esp/esp-idf/components/app_trace/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/aws_iot/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/bt/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/driver/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/esp-mqtt/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/esp32/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/ethernet/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/fatfs/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/freemodbus/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/freertos/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/heap/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/http_server/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/libsodium/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/log/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/lwip/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/mbedtls/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/mdns/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/mqtt/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/openssl/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/pthread/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/spi_flash/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/spiffs/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/vfs/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/Labor.GTC/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/Labor.GTC/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/d/Users/Labor.GTC/Desktop/Esp32IoT/main/Kconfig.projbuild \
	/home/Labor.GTC/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/Labor.GTC/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
