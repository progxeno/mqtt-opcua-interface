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
//static bool status = true;

static uint8_t crc_array[256] = {
0x00, 0x2F, 0x5E, 0x71, 0xBC, 0x93, 0xE2, 0xCD, 0x57, 0x78, 0x09, 0x26,
0xEB, 0xC4, 0xB5, 0x9A, 0xAE, 0x81, 0xF0, 0xDF, 0x12, 0x3D, 0x4C, 0x63,
0xF9, 0xD6, 0xA7, 0x88, 0x45, 0x6A, 0x1B, 0x34, 0x73, 0x5C, 0x2D, 0x02,
0xCF, 0xE0, 0x91, 0xBE, 0x24, 0x0B, 0x7A, 0x55, 0x98, 0xB7, 0xC6, 0xE9,
0xDD, 0xF2, 0x83, 0xAC, 0x61, 0x4E, 0x3F, 0x10, 0x8A, 0xA5, 0xD4, 0xFB,
0x36, 0x19, 0x68, 0x47, 0xE6, 0xC9, 0xB8, 0x97, 0x5A, 0x75, 0x04, 0x2B,
0xB1, 0x9E, 0xEF, 0xC0, 0x0D, 0x22, 0x53, 0x7C, 0x48, 0x67, 0x16, 0x39,
0xF4, 0xDB, 0xAA, 0x85, 0x1F, 0x30, 0x41, 0x6E, 0xA3, 0x8C, 0xFD, 0xD2,
0x95, 0xBA, 0xCB, 0xE4, 0x29, 0x06, 0x77, 0x58, 0xC2, 0xED, 0x9C, 0xB3,
0x7E, 0x51, 0x20, 0x0F, 0x3B, 0x14, 0x65, 0x4A, 0x87, 0xA8, 0xD9, 0xF6,
0x6C, 0x43, 0x32, 0x1D, 0xD0, 0xFF, 0x8E, 0xA1, 0xE3, 0xCC, 0xBD, 0x92,
0x5F, 0x70, 0x01, 0x2E, 0xB4, 0x9B, 0xEA, 0xC5, 0x08, 0x27, 0x56, 0x79,
0x4D, 0x62, 0x13, 0x3C, 0xF1, 0xDE, 0xAF, 0x80, 0x1A, 0x35, 0x44, 0x6B,
0xA6, 0x89, 0xF8, 0xD7, 0x90, 0xBF, 0xCE, 0xE1, 0x2C, 0x03, 0x72, 0x5D,
0xC7, 0xE8, 0x99, 0xB6, 0x7B, 0x54, 0x25, 0x0A, 0x3E, 0x11, 0x60, 0x4F,
0x82, 0xAD, 0xDC, 0xF3, 0x69, 0x46, 0x37, 0x18, 0xD5, 0xFA, 0x8B, 0xA4,
0x05, 0x2A, 0x5B, 0x74, 0xB9, 0x96, 0xE7, 0xC8, 0x52, 0x7D, 0x0C, 0x23,
0xEE, 0xC1, 0xB0, 0x9F, 0xAB, 0x84, 0xF5, 0xDA, 0x17, 0x38, 0x49, 0x66,
0xFC, 0xD3, 0xA2, 0x8D, 0x40, 0x6F, 0x1E, 0x31, 0x76, 0x59, 0x28, 0x07,
0xCA, 0xE5, 0x94, 0xBB, 0x21, 0x0E, 0x7F, 0x50, 0x9D, 0xB2, 0xC3, 0xEC,
0xD8, 0xF7, 0x86, 0xA9, 0x64, 0x4B, 0x3A, 0x15, 0x8F, 0xA0, 0xD1, 0xFE,
0x33, 0x1C, 0x6D, 0x42
};

static uint8_t compute_crc(uint8_t *byte, size_t size) {
    uint8_t  crc = 0xFF;

    for (size_t i = 0; i < size; ++i) {
        crc = crc_array[crc ^ byte[i]];
    }
    crc = ~crc;

    return crc;
}
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

	 if (crc == compute_crc(rx, sizeof(rx)-1)) {
	        if (error > 1) {
	            phase = (double)alpha * factor;
	            *value = phase;
	        } else {
	        	ret = ESP_ERR_NOT_FOUND;
	        }
	    } else {
	    	ret = ESP_ERR_NOT_FOUND;
	    }

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

