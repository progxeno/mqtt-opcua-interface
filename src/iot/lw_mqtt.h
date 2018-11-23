/*
 * rpi_mqtt.h
 *
 *  Created on: Oct 24, 2018
 *      Author: Labor.GTC
 */

#ifndef IOT_RPI_MQTT_H_
#define IOT_RPI_MQTT_H_

#include <stdlib.h>
#include <esp_mqtt.h>

#include "cJSON.h"
#include "../tls/mqttMbedtls.h"


//#include "../driver/mb1222.h"

#define WIFI_SSID "smc@iot"
#define WIFI_PASS "12345678iot"

#define MQTT_HOST "10.0.0.1"
#define MQTT_PORT "1883"
#define MQTT_USER "device"
#define MQTT_PASS "device@mqtt"


static bool status = true;

static void json_test()
{
    /*{
        "name": "Mars",
        "mass": 639e21,
        "moons": [
            {
                "name": "Phobos",
                "size": 70
            },
            {
                "name": "Deimos",
                "size": 39
            }
        ]
    }*/

    char *strJson = "{\"name\" : \"Mars\",\"mass\":639e21,\"moons\":[{\"name\":\"Phobos\",\"size\":70},{\"name\":\"Deimos\",\"size\":39}]}";

    printf("Planet:\n");
    // First, parse the whole thing
    cJSON *root = cJSON_Parse(strJson);
    // Let's get some values
    char *name = cJSON_GetObjectItem(root, "name")->valuestring;
    double mass = cJSON_GetObjectItem(root, "mass")->valuedouble;
    printf("%s, %.2e kgs\n", name, mass); // Note the format! %.2e will print a number with scientific notation and 2 decimals
    // Now let's iterate through the moons array
    cJSON *moons = cJSON_GetObjectItem(root, "moons");
    // Get the count
    int moons_count = cJSON_GetArraySize(moons);
    int i;
    for (i = 0; i < moons_count; i++) {
        printf("Moon:\n");
        // Get the JSON element and then get the values as before
        cJSON *moon = cJSON_GetArrayItem(moons, i);
        char *name = cJSON_GetObjectItem(moon, "name")->valuestring;
        int size = cJSON_GetObjectItem(moon, "size")->valueint;
        printf("%s, %d kms\n", name, size);
    }

    // Finally remember to free the memory!
    cJSON_Delete(root);
}

static void mqtt_process_task(void *p)
{
    ESP_ERROR_CHECK(i2c_master_init());

    int ret;
    char buf[10];
    uint8_t sensor_data_h, sensor_data_l;
    uint16_t sensor_data;

	while (1)
	{
		//fputs(status ? "true" : "false", stdout);

        ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data_h, &sensor_data_l);
        sensor_data = (uint16_t) sensor_data_h << 8 | sensor_data_l;
        sprintf (buf, "%u", sensor_data);

        if (status)
		{
	        if (ret == ESP_ERR_TIMEOUT) {
	            ESP_LOGE(TAG, "I2C Timeout");
	        } else if (ret == ESP_OK) {
	    		esp_mqtt_publish("device/id1/data/range", (uint8_t *) buf, 10, 1, false);
	        } else {
	            ESP_LOGW(TAG, "%s: No ack, sensor not connected. ", esp_err_to_name(ret));
	        }
			json_test();
		}
	}
}

static void mqtt_restart_task(void *_)
{
	while (1)
	{
		// stop and start mqtt every minute
		vTaskDelay(60000 / portTICK_PERIOD_MS); // block for 60000msec -> 60sec => 1min
		esp_mqtt_start(MQTT_HOST, MQTT_PORT, "esp-mqtt", MQTT_USER, MQTT_PASS);
	}
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id)
	{
	case SYSTEM_EVENT_STA_START:
		// connect to ap
		esp_wifi_connect();
		break;

	case SYSTEM_EVENT_STA_GOT_IP:
		// start mqtt
		esp_mqtt_start(MQTT_HOST, MQTT_PORT, "esp-mqtt", MQTT_USER, MQTT_PASS);
		break;

	case SYSTEM_EVENT_STA_DISCONNECTED:
		// stop mqtt
		esp_mqtt_stop();

		// reconnect wifi
		esp_wifi_connect();
		break;

	default:
		break;
	}

	return ESP_OK;
}

static void status_callback(esp_mqtt_status_t status)
{
	switch (status)
	{
	case ESP_MQTT_STATUS_CONNECTED:
		// subscribe
		esp_mqtt_subscribe("master/id1/message", 1);
		break;
	case ESP_MQTT_STATUS_DISCONNECTED:
		// reconnect
		esp_mqtt_start(MQTT_HOST, MQTT_PORT, "esp-mqtt", MQTT_USER, MQTT_PASS);
		break;
	}
}

static void message_callback(const char *topic, uint8_t *payload, size_t len)
{
	if (atoi((char *)payload) == 0)
	{
		status = false;
	}
	else if (atoi((char *)payload) == 1)
	{
		status = true;
	}

	ESP_LOGI("test", "incoming: %s => %s (%d)", topic, payload, (int)len);
}


#endif /* IOT_RPI_MQTT_H_ */
