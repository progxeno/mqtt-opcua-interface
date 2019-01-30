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

//#include "esp_wifi.h"
//#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_event_loop.h"
//#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "rom/cache.h"
#include "driver/i2c.h"
#include "esp_log.h"

//static char *TAG = "I2C";

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

//static bool status = true;
/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init();

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
esp_err_t i2c_master_read_sensor(i2c_port_t i2c_num, uint16_t *data);

#endif /* DRIVER_MB1222_H_ */
