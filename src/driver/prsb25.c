/*
 * prsb25.c
 *
 *  Created on: 30.01.2019
 *      Author: miosga.mario
 */

#include "prsb25.h"

uint8_t compute_crc(uint8_t *byte, size_t size) {
    uint8_t  crc = 0xFF;

    for (size_t i = 0; i < size; ++i) {
        crc = crc_array[crc ^ byte[i]];
    }
    crc = ~crc;

    return crc;
}
/// Initialize the SPI2 device in master mode
esp_err_t spi_master_config(void)
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

esp_err_t spi_master_read_sensor(double *value)
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
	vTaskDelay(pdMS_TO_TICKS(1));

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
	vTaskDelay(pdMS_TO_TICKS(1));

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
