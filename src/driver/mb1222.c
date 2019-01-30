/*
 * mb1222.c
 *
 *  Created on: 30.01.2019
 *      Author: miosga.mario
 */

#include "mb1222.h"

esp_err_t i2c_master_init()
{
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
	I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
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
esp_err_t i2c_master_read_sensor(i2c_port_t i2c_num, uint16_t *data)
{
	int ret;
	uint8_t data_h;
	uint8_t data_l;

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
	vTaskDelay(pdMS_TO_TICKS(50));

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MB1222_SENSOR_ADDR << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data_h, ACK_VAL);
	i2c_master_read_byte(cmd, &data_l, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 10 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	*data = (uint16_t) data_h << 8 | data_l;

	return ret;
}
