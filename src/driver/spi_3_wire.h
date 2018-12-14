/*
 * spi_3_wire.h
 *
 *  Created on: 13.12.2018
 *      Author: miosga.mario
 */

#ifndef SRC_DRIVER_SPI_3_WIRE_H_
#define SRC_DRIVER_SPI_3_WIRE_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "rom/cache.h"
#include "driver/spi_master.h"
#include "esp_log.h"
//#include "esp_spi_flash.h"

static char *TAG = "spi";
/*
 * SPI master modes for ESP32:
 * - Mode 0: CPOL = 0, CPHA = 0
 * - Mode 1: CPOL = 0, CPHA = 1
 * - Mode 2: CPOL = 1, CPHA = 1
 * - Mode 3: CPOL = 1, CPHA = 0
 */
#define SPI_MODE       2 // Default SPI mode 2

#define PIN_NUM_DATA 19
#define PIN_NUM_CLK 18
#define PIN_NUM_SS   5

//#define SPI_CLK_GPIO   GPIO_NUM_6 // Shared with JTAG/MicroSD
//#define SPI_MISO_GPIO  GPIO_NUM_7 // Shared with JTAG/MicroSD
//#define SPI_MOSI_GPIO  GPIO_NUM_8 // Shared with JTAG/MicroSD

static uint8_t myRxBuffer[SPI_MAX_DMA_LEN] = { };
static uint8_t myTxBuffer[SPI_MAX_DMA_LEN] = { };

spi_device_handle_t spi_handle;
static bool status = true;

// Initialize the SPI2 device in master mode
static esp_err_t spi_master_config(void) {

	esp_err_t ret;
	// Configuration for the SPI bus
	spi_bus_config_t buscfg = { .mosi_io_num = -1, .miso_io_num = PIN_NUM_DATA,
			.sclk_io_num = PIN_NUM_CLK, .quadwp_io_num = -1,
			.quadhd_io_num = -1, .max_transfer_sz = SPI_MAX_DMA_LEN, };

	// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = { .clock_speed_hz = 40 * 1000 * 1000,
			.mode = SPI_MODE, .spics_io_num = PIN_NUM_SS, .queue_size = 1,
			.flags = SPI_DEVICE_3WIRE, // | SPI_DEVICE_HALFDUPLEX,
			.pre_cb = NULL, .post_cb = NULL, };

	// Initialize and enable SPI

	ret = spi_bus_initialize(VSPI_HOST, &buscfg, 1);
//	ESP_ERROR_CHECK(ret);

	ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
//	ESP_ERROR_CHECK(ret);
	return ret;
}

static esp_err_t spi_master_read_sensor(uint8_t *data, uint8_t *size) {

	int ret;
	spi_transaction_t trans_t;
	spi_transaction_t trans_r;
	memset(&trans_t, 0, sizeof(trans_t));
	memset(&trans_r, 0, sizeof(trans_r));
	// Prepare transaction parameter
	char cmd[] = { 0xAA, 0xFF };
	char rxData[2] = {0};

	trans_t.length = 2*8;
	trans_t.tx_buffer = cmd;
	trans_r.length = 2*8;
	trans_r.rx_buffer = rxData;

	ret = spi_device_transmit(spi_handle, &trans_t);
	if (ESP_OK != ret) {
		return ret;
	}
	ret = spi_device_transmit(spi_handle, &trans_r);
	if (ESP_OK != ret) {
		return ret;
	}

	ESP_LOGI(TAG, "Data0: %i", rxData[0]);
	ESP_LOGI(TAG, "Data1: %i", rxData[1]);

	return ret;

//		if (data == myRxBuffer) {
//			trans_t.rx_buffer = myRxBuffer;
//			trans_t.tx_buffer = NULL;
//			trans_t.tx_data[0] = data[0];
//			trans_t.tx_data[1] = data[1];
//
//		} else {
//			trans_t.rx_buffer = NULL;
//			trans_t.tx_buffer = 2;
//			trans_t.tx_data[0] = 0xaa;
//			trans_t.tx_data[1] = 0xff;
//
//		}
//		trans_t.rxlength = 0;
//		trans_t.length = 8 * (*size);
//		trans_t.flags = 0;
//		trans_t.cmd = 0;
//		trans_t.addr = 0;
//		trans_t.user = NULL;
//
//		//ESP_LOGI(TAG, "rxbuffer: %s",trans_t.rx_data);
//		ESP_LOGI(TAG, "txbuffer0: %i",trans_t.tx_data[0]);
//		ESP_LOGI(TAG, "txbuffer1: %i",trans_t.tx_data[1]);
//
//		// Perform transaction
//		ret = spi_device_transmit(spi_handle,&trans_t);
//		if (ESP_OK != ret) {
//			return ret;
//		}
//		data = trans_t.rx_data[0];
//		return ret;
}

// Full buffer DMA transfer
static void spi_process_task(void *arg) {

	ESP_ERROR_CHECK(spi_master_config());
	int ret;
	char buf[10];

	uint8_t sensor_data_h, sensor_data_l;
	uint8_t sensor_data;
	while (1) {

		ret = spi_master_read_sensor(&sensor_data_h, &sensor_data_l);
		ret = spi_master_read_sensor(&sensor_data_h, &sensor_data_l);
		vTaskDelay(0.3 / portTICK_RATE_MS);
		sensor_data = sensor_data_h << 8 | sensor_data_l;
		sprintf(buf, "%u", sensor_data);
		ESP_LOGW(TAG, "\nSens: %u\n", sensor_data);
		if (ret == ESP_ERR_TIMEOUT) {
			ESP_LOGE(TAG, "SPI Timeout");
		} else if (ret == ESP_OK) {
			ESP_LOGI(TAG, "\nsensor val: %s\n", buf);
//				printf("\nsensor val: %i [cm]\n",
//						sensor_data_h << 8 | sensor_data_l);
		} else {
			ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...",
					esp_err_to_name(ret));
		}
	}
	vTaskDelete(NULL);
}

#endif /* SRC_DRIVER_SPI_3_WIRE_H_ */

///*
// * spi_3_wire.h
// *
// *  Created on: 13.12.2018
// *      Author: miosga.mario
// */
//
//#ifndef SRC_DRIVER_SPI_3_WIRE_H_
//#define SRC_DRIVER_SPI_3_WIRE_H_
//
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
//#define SPI_MODE       2 // Default SPI mode 2
//
//#define PIN_NUM_DATA 19
//#define PIN_NUM_CLK 18
//#define PIN_NUM_SS   5
//
////#define SPI_CLK_GPIO   GPIO_NUM_6 // Shared with JTAG/MicroSD
////#define SPI_MISO_GPIO  GPIO_NUM_7 // Shared with JTAG/MicroSD
////#define SPI_MOSI_GPIO  GPIO_NUM_8 // Shared with JTAG/MicroSD
//
////uint8_t myRxBuffer[SPI_MAX_DMA_LEN] = { };
////uint8_t myTxBuffer[SPI_MAX_DMA_LEN] = { };
//
//spi_device_handle_t spi_handle;
//static bool status = true;
//
//// Initialize the SPI2 device in master mode
//static esp_err_t spi_master_config(void) {
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
//			.clock_speed_hz = 10 * 1000 * 1000,
//			.mode = SPI_MODE,
//			.spics_io_num = PIN_NUM_SS,
//			.queue_size = 1,
//			.flags = SPI_DEVICE_3WIRE | SPI_DEVICE_HALFDUPLEX,
//			.pre_cb = NULL,
//			.post_cb = NULL, };
//
//
//	// Initialize and enable SPI
//	ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
//	ESP_ERROR_CHECK(ret);
//	ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle);
//	ESP_ERROR_CHECK(ret);
//	return ret;
//}
//
//static esp_err_t spi_master_read_sensor(uint16_t data, uint16_t size) {
//
//	int ret;
//		spi_transaction_t trans_t;
//
//		// Prepare transaction parameters
//		if (data == 2) {
//			trans_t.rx_buffer = 2;
//			trans_t.tx_buffer = NULL;
//		} else {
//			trans_t.rx_buffer = NULL;
//			trans_t.tx_buffer = 2;
//		}
//		trans_t.tx_data[0] = 0xff,
//		trans_t.rxlength = 0;
//		trans_t.length = 8 * size;
//		trans_t.flags = 0;
//		trans_t.cmd = 0;
//		trans_t.addr = 0;
//		trans_t.tx_data[0] = 0xff,
//		trans_t.user = NULL;
//
//		// Perform transaction
//		//ret = spi_device_transmit(spi_handle, &trans_t);
//		ret = spi_device_transmit(spi_handle,&trans_t);
//		if (ESP_OK != ret) {
//			return ret;
//		}
//		 data = trans_t.rx_data[0];
//		return ret;
//}
//
//static esp_err_t spi_master_write_sensor(uint16_t data, uint16_t size) {
//
//	int ret;
//		spi_transaction_t trans_t;
//
//		// Prepare transaction parameters
//		trans_t.length = 8 * size;
//		trans_t.flags = 0;
//		trans_t.addr = 0;
//		trans_t.tx_data[0] = data;
//
//		// Perform transaction
//		//ret = spi_device_transmit(spi_handle, &trans_t);
//		ret = spi_device_transmit(spi_handle,&trans_t);
//		if (ESP_OK != ret) {
//			ESP_LOGE(TAG, "Send buffer failed, err = %d", ret);
//			return ret;
//		}
//		return ret;
//}
//
//
//// Full buffer DMA transfer
//static void spi_process_task(void *arg) {
//
//	int ret;
//		uint8_t sensor_data_h, sensor_data_l;
//
//		while (1) {
//			ret = spi_master_read_sensor(sensor_data_h,
//					sensor_data_l);
//			ret = spi_master_write_sensor(sensor_data_h,
//								sensor_data_l);
//			if (ret == ESP_ERR_TIMEOUT) {
//				ESP_LOGE(TAG, "SPI Timeout");
//			} else if (ret == ESP_OK) {
//				printf("\nsensor val: %i [cm]\n",
//						sensor_data_h << 8 | sensor_data_l);
//			} else {
//				ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...",
//						esp_err_to_name(ret));
//			}
//		}
//		vTaskDelete (NULL);
//}
//
//#endif /* SRC_DRIVER_SPI_3_WIRE_H_ */
