///*
// * spi_3_wire.h
// *
// *  Created on: 13.12.2018
// *      Author: miosga.mario
// */
//
//#ifndef SRC_DRIVER_VERT_X_13_H_
//#define SRC_DRIVER_VERT_X_13_H_
//
//#include <stdio.h>
//#include <stdint.h>
//#include <stddef.h>
//#include <string.h>
//
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "freertos/semphr.h"
//#include "freertos/queue.h"
//
//#include "lwip/sockets.h"
//#include "lwip/dns.h"
//#include "lwip/netdb.h"
//#include "lwip/igmp.h"
//
//#include "esp_wifi.h"
//#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_event_loop.h"
//#include "nvs_flash.h"
//#include "soc/rtc_cntl_reg.h"
//#include "rom/cache.h"
//#include "driver/spi_master.h"
//#include "esp_log.h"
////#include "esp_spi_flash.h"
//
//static char *TAG = "spi";
///*
// * SPI master modes for ESP32:
// * - Mode 0: CPOL = 0, CPHA = 0
// * - Mode 1: CPOL = 0, CPHA = 1
// * - Mode 2: CPOL = 1, CPHA = 1
// * - Mode 3: CPOL = 1, CPHA = 0
// */
//#define SPI_MODE		0 // Default SPI mode 2
//
//#define PIN_NUM_DATA	19
//#define PIN_NUM_CLK		18
//#define PIN_NUM_SS		5
//
//spi_device_handle_t spi_handle;
//static bool status = true;
//
//// Initialize the SPI2 device in master mode
//static esp_err_t spi_master_config(void)
//{
//
//	esp_err_t ret;
//	// Configuration for the SPI bus
//	spi_bus_config_t buscfg = {
//			.mosi_io_num = -1,
//			.miso_io_num = PIN_NUM_DATA,
//			.sclk_io_num = PIN_NUM_CLK,
//			.quadwp_io_num = -1,
//			.quadhd_io_num = -1,
//			.max_transfer_sz = SPI_MAX_DMA_LEN, };
//
//	// Configuration for the SPI master interface
//	spi_device_interface_config_t devcfg = {
//			.clock_speed_hz = 500000,
//			.mode = SPI_MODE,
//			.spics_io_num = PIN_NUM_SS,
//			.queue_size = 1,
//			.flags = SPI_DEVICE_3WIRE, // | SPI_DEVICE_HALFDUPLEX,
//			.pre_cb = NULL,
//			.post_cb = NULL, };
//
//	// Initialize and enable SPI
//	ret = spi_bus_initialize(VSPI_HOST, &buscfg, 1);
////	ESP_ERROR_CHECK(ret);
//
//	ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
////	ESP_ERROR_CHECK(ret);
//
////	gpio_set_direction(PIN_NUM_DATA, GPIO_MODE_INPUT_OUTPUT);
//
//	return ret;
//}
//
//static esp_err_t spi_master_read_sensor(uint8_t *data, uint8_t *size)
//{
//
//	int ret;
//	// Prepare transaction parameter
//	uint8_t tx[2] = {
//			0xAA,
//			0xFF };
//	uint8_t rx[2] = {
//			0 };
//
//	spi_transaction_t trans;
//	memset(&trans, 0, sizeof(trans));
//	trans.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
//	trans.length = 2 * 8;
//	trans.rxlength = 2 * 8;
//	trans.tx_data[0] = 0xAA;
//	trans.tx_data[1] = 0xFF;
//
//	ret = spi_device_transmit(spi_handle, &trans);
//	if (ESP_OK != ret) {
//		return ret;
//	}
//
//	vTaskDelay(0.3 / portTICK_RATE_MS);
//
////	ESP_LOGI(TAG, "Data0: %i", rx[0]);
////	ESP_LOGI(TAG, "Data1: %i", rx[1]);
//	ESP_LOGI(TAG, "Data0: %i", trans.rx_data[0]);
//	ESP_LOGI(TAG, "Data1: %i", trans.rx_data[1]);
//
//	return ret;
//}
//
//// Full buffer DMA transfer
//static void spi_process_task(void *arg)
//{
//
//	ESP_ERROR_CHECK(spi_master_config());
//	int ret;
//	char buf[10];
//
//	uint8_t sensor_data_h, sensor_data_l;
//	uint8_t sensor_data;
//	while (1) {
//		ret = spi_master_read_sensor(&sensor_data_h, &sensor_data_l);
//		sensor_data = sensor_data_h << 8 | sensor_data_l;
//		sprintf(buf, "%u", sensor_data);
////		ESP_LOGW(TAG, "\nSens: %u\n", sensor_data);
//		if (ret == ESP_ERR_TIMEOUT) {
//			ESP_LOGE(TAG, "SPI Timeout");
//		} else if (ret == ESP_OK) {
////			ESP_LOGI(TAG, "\nsensor val: %s\n", buf);
////				printf("\nsensor val: %i [cm]\n",
////						sensor_data_h << 8 | sensor_data_l);
//		} else {
//			ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...", esp_err_to_name(ret));
//		}
//	}
//	vTaskDelete(NULL);
//}
//
//#endif /* SRC_DRIVER_VERT_X_13_H_ */
