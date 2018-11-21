/* Blink Example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>

#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "../src/driver/mb1222.h"
#include "../src/iot/rpi_mqtt.h"
#include "../src/tls/certificate.h"
#include "../src/tls/mqttMbedtls.h"

void app_main()
{
	// initialize nvs flash
	ESP_ERROR_CHECK(nvs_flash_init());

	// initialize tcp/ip adapter
	tcpip_adapter_init();

	// register event handler
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	// initialize wifi
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// set wifi storage to ram
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	// set wifi mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// prepare wifi config
	wifi_config_t wifi_config = { .sta = { .ssid = WIFI_SSID, .password = WIFI_PASS } };
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	// start wifi
	esp_err_t wifiState = esp_wifi_start();
	ESP_ERROR_CHECK(wifiState);

	// passed value from i2c_process_task to mqtt_process_task
	//static uint8_t *sonar;

	// i2c slave
    //ESP_ERROR_CHECK(i2c_master_init());
    //xTaskCreatePinnedToCore(i2c_process_task, "i2c_process_task", 2048, (void *)sonar, 10, NULL, 0);

	// mqtt initialize
	esp_mqtt_init(status_callback, message_callback, 512, 1000);
	// mqtt create tasks
	xTaskCreatePinnedToCore(mqtt_process_task, "process_task", 1024 * 2, NULL, 10, NULL, 1);
	//askCreatePinnedToCore(mqtt_restart_task, "restart_task", 1024, NULL, 10, NULL, 1);
}
