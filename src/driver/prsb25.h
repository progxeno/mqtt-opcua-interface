/*
 * prsb25.h
 *
 *  Created on: 04.01.2019
 *      Author: kloos.alexej
 */

#ifndef SRC_DRIVER_PRSB25_H_
#define SRC_DRIVER_PRSB25_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

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
#define SPI_MODE		1
#define PIN_NUM_MOSI	23
#define PIN_NUM_MISO	19
#define PIN_NUM_CLK		18
#define PIN_NUM_SS		5

spi_device_handle_t spi_handle;
bool status = true;

/// Initialize the SPI2 device in master mode
static esp_err_t spi_master_config(void)
{
	esp_err_t ret;
	/// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
			.mosi_io_num = PIN_NUM_MOSI,
			.miso_io_num = PIN_NUM_MISO,
			.sclk_io_num = PIN_NUM_CLK,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
			.max_transfer_sz = SPI_MAX_DMA_LEN, };

	/// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = {
			.clock_speed_hz = 500000,
			.mode = SPI_MODE,
			.spics_io_num = PIN_NUM_SS,
			.queue_size = 1,
//			.pre_cb = NULL,
//			.post_cb = NULL,
			};

	/// Initialize and enable SPI
	ret = spi_bus_initialize(VSPI_HOST, &buscfg, 1);
//	ESP_ERROR_CHECK(ret);

	ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
//	ESP_ERROR_CHECK(ret);

	return ret;
}

static esp_err_t spi_master_read_sensor(double *value)
{
	int ret = -1;
	double phase = 0.0;
	const double factor = 360.0 / 16383.0;
	/// Block for 1ms.
	const TickType_t xDelay = 1 / portTICK_PERIOD_MS;

	/// Prepare transaction parameter
	uint8_t tx[] = {
			0x00,
			0x00,
			0xFF,
			0xFF,
			0x00,
			0x00,
			0x13,
			0xEA };
	uint8_t rx[8] = {
			0 };

	spi_transaction_t trans;
	memset(&trans, 0, sizeof(trans));
	trans.length = 8 * 8;
	trans.rxlength = 8 * 8;
	trans.tx_buffer = &tx;
	trans.rx_buffer = &rx;

	ret = spi_device_transmit(spi_handle, &trans);
	if (ESP_OK != ret) {
		return ret;
	}

	/// Mandatory
	vTaskDelay(xDelay);

	/// NOP
	tx[0] = 0x00;
	tx[1] = 0x00;
	tx[2] = 0xAA;
	tx[3] = 0xAA;
	tx[4] = 0x00;
	tx[5] = 0x00;
	tx[6] = 0xD0;
	tx[7] = 0xAB;

	ret = spi_device_transmit(spi_handle, &trans);
	if (ESP_OK != ret) {
		return ret;
	}

	/// Mandatory
	vTaskDelay(xDelay);

	/// Extract and convert the angle to degrees
	unsigned int alpha = rx[0] | (rx[1] & 0x3F) << 8;
	/// Extract the error bits
	uint8_t error = rx[1] >> 6;
	/// Extract the virtual gain byte
	uint8_t gain = rx[4];
	/// Extract the rolling counter
	uint8_t counter = rx[6] & 0x3F;
	/// Extract the CRC
	uint8_t crc = rx[7];

	//vTaskDelay(500 / portTICK_PERIOD_MS);

//    ESP_LOGI(TAG, "crc %d\talpha %d\tcount %2d\n", crc, alpha, counter);
//    ESP_LOGI(TAG, "error %d\tphase %2.5f\n", error, phase);

	phase = (double) alpha * factor;

	*value = phase;
//	ESP_LOGI(TAG, "Data: %f\n", phase);

	return ret;
}

/// Full buffer DMA transfer
//static void spi_process_task(void *arg)
//{
//	int ret;
//	char buf[10];
//	double sensor_data;
//
//	ESP_ERROR_CHECK(spi_master_config());
//
//	while (1) {
//		ret = spi_master_read_sensor(&sensor_data);
//		sprintf(buf, "%f", sensor_data);
//		if (ret == ESP_ERR_TIMEOUT) {
//			ESP_LOGE(TAG, "SPI Timeout");
//		} else if (ret == ESP_OK) {
//			ESP_LOGI(TAG, "\nsensor val: %s\n", buf);
//		} else {
//			ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...", esp_err_to_name(ret));
//		}
//	}
//	vTaskDelete(NULL);
//}

#endif /// SRC_DRIVER_PRSB25_H_

