/*
 * mb1222.h
 *
 *  Created on: Oct 24, 2018
 *      Author: Labor.GTC
 * For other examples please check:
 * https://github.com/espressif/esp-idf/tree/master/examples
 *
 * See README.md file to get detailed usage of this example.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 */

#ifndef DRIVER_MB1222_H_
#define DRIVER_MB1222_H_

#ifndef MQTT_USE_TLS
#define MQTT_USE_TLS
#endif

#include <driver/i2c.h>

#include "esp_log.h"

static char *TAG = "i2c";

#define I2C_MASTER_SCL_IO			22				/*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO			21				/*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM				0				/*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ			100000			/*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE	0				/*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE	0				/*!< I2C master doesn't need buffer */

#define MB1222_SENSOR_ADDR	0x70					/*!< slave address for sensor */
#define WRITE_BIT			0x0						/*!< I2C master write */
#define READ_BIT			0x1						/*!< I2C master read */
#define ACK_CHECK_EN		0x1						/*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS		0x0						/*!< I2C master will not check ack from slave */
#define ACK_VAL				0x0						/*!< I2C ack value */
#define NACK_VAL			0x1						/*!< I2C nack value */

static bool status = true;
/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init() {
	int i2c_master_port = I2C_MASTER_NUM;
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_MASTER_SDA_IO;
	conf.scl_io_num = I2C_MASTER_SCL_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
	i2c_param_config(i2c_master_port, &conf);
	return i2c_driver_install(i2c_master_port, conf.mode,
	I2C_MASTER_RX_BUF_DISABLE,
								I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief test code to operate on MB1222 sensor
 *
 * 1. set operation mode(e.g One time L-resolution mode)
 * _________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|------|
 * 2. wait more than 100 ms
 * 3. read data
 * ______________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read 1 byte + ack  | read 1 byte + nack | stop |
 * --------|---------------------------|--------------------|--------------------|------|
 */
static esp_err_t i2c_master_read_sensor(i2c_port_t i2c_num, uint16_t *data) {
	int ret;
	uint8_t *data_h = 0;
	uint8_t *data_l = 0;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MB1222_SENSOR_ADDR << 1 | WRITE_BIT,
	ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x51, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 10 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (ret != ESP_OK) {
		return ret;
	}
	vTaskDelay(100 / portTICK_RATE_MS);
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MB1222_SENSOR_ADDR << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, data_h, ACK_VAL);
	i2c_master_read_byte(cmd, data_l, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 10 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	*data = (uint16_t) *data_h << 8 | *data_l;

	return ret;
}

static void i2c_process_task(void *arg)
{
	int ret;
	uint16_t sensor_data;

	while (1) {
		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data);

		if (ret == ESP_ERR_TIMEOUT) {
			ESP_LOGE(TAG, "I2C Timeout");
		} else if (ret == ESP_OK) {
			printf("\nsensor val: %i [cm]\n", sensor_data);
		} else {
			ESP_LOGW(TAG, "%s: No ack, sensor not connected...skip...", esp_err_to_name(ret));
		}
	}
	vTaskDelete (NULL);
}

#endif /* DRIVER_MB1222_H_ */
