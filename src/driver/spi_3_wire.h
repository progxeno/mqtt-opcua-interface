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
#include "driver/spi_slave.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
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

uint8_t myRxBuffer[SPI_MAX_DMA_LEN] = { };
uint8_t myTxBuffer[SPI_MAX_DMA_LEN] = { };

spi_device_handle_t spi_handle;

// Initialize the SPI2 device in master mode
void spi_master_config(void) {

	esp_err_t ret;
    spi_device_handle_t spi;

	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
			.mosi_io_num = -1,
			.miso_io_num = SPI_DATA_GPIO,
			.sclk_io_num = SPI_CLK_GPIO,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
			.max_transfer_sz = SPI_MAX_DMA_LEN, };

	// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = {
			.clock_speed_hz = 10 * 1000 * 1000,
			.mode = SPI_MODE,
			.queue_size = 1,
			.flags = SPI_DEVICE_3WIRE | SPI_DEVICE_HALFDUPLEX,
			.pre_cb = NULL,
			.post_cb = NULL, };
	//Configuration for the handshake line
	      gpio_config_t io_conf={
	          .intr_type=GPIO_INTR_DISABLE,
	          .mode=GPIO_MODE_OUTPUT,
	          .pin_bit_mask=(1<<SPI_MODE)
	      };


	// Initialize and enable SPI
	spi_bus_initialize(SPI_HOST, &buscfg, 1);
	spi_bus_add_device(SPI_HOST, &devcfg, &spi_handle);
}

// Full buffer DMA transfer
int32_t spi_dma_transfer_bytes(uint8_t *data, uint16_t size) {

	esp_err_t ret;
    spi_device_handle_t spi;

	esp_err_t trans_result = ESP_OK;
	spi_transaction_t trans_t;

	// Prepare transaction parameters
	if (data == myRxBuffer) {
		trans_t.rx_buffer = myRxBuffer;
		trans_t.tx_buffer = NULL;
	} else {
		trans_t.rx_buffer = NULL;
		trans_t.tx_buffer = myTxBuffer;
	}
	trans_t.rxlength = 0;
	trans_t.length = 8 * size;
	trans_t.flags = 0;
	trans_t.cmd = 0;
	trans_t.addr = 0;
	trans_t.user = NULL;

	// Perform transaction
	trans_result = spi_device_transmit(spi_handle, &trans_t);
	if (ESP_OK != trans_result) {
		return 0;
	}

	return size;
}

#endif /* SRC_DRIVER_SPI_3_WIRE_H_ */
