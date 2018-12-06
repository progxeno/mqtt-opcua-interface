///*
// * lw_mbedtls_mqtt.c
// *
// *  Created on: 06.12.2018
// *      Author: Mario
// */
//#include "lw_mbedtls_mqtt.h"
//
////int TLS_Connect(Network* n, char* addr, int port) {
////    char portbuf[100];
////    int ret, flags;
////	int retVal = -1;
////	int len;
////
////    mbedtls_ssl_init(&n->ssl);
////    mbedtls_x509_crt_init(&n->cacert);
////    mbedtls_ctr_drbg_init(&n->ctr_drbg);
////    ESP_LOGD(TAG, "Seeding the random number generator");
////
////    mbedtls_ssl_config_init(&n->conf);
////
////    mbedtls_entropy_init(&n->entropy);
////    if((ret = mbedtls_ctr_drbg_seed(&n->ctr_drbg, mbedtls_entropy_func, &n->entropy,
////                                    NULL, 0)) != 0)
////    {
////        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
////        abort();
////    }
////
////
////    ESP_LOGD(TAG, "Loading the CA root certificate...");
////
////    ret = mbedtls_x509_crt_parse(&n->cacert, (uint8_t*)server_root_cert, strlen(server_root_cert)+1);
////    if(ret < 0)
////    {
////        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
////        abort();
////    }
////
////    ESP_LOGD(TAG, "Setting hostname for TLS session...");
////
////     /* Hostname set here should match CN in server certificate */
////    if((ret = mbedtls_ssl_set_hostname(&n->ssl, addr)) != 0)
////    {
////        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
////        abort();
////    }
////
////    ESP_LOGD(TAG, "Setting up the SSL/TLS structure...");
////
////    if((ret = mbedtls_ssl_config_defaults(&n->conf,
////                                          MBEDTLS_SSL_IS_CLIENT,
////                                          MBEDTLS_SSL_TRANSPORT_STREAM,
////                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
////    {
////        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
////        goto exit;
////    }
////
////    /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
////       a warning if CA verification fails but it will continue to connect.
////
////       You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
////    */
////    mbedtls_ssl_conf_authmode(&n->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
////    // mbedtls_ssl_conf_authmode(&n->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
////    mbedtls_ssl_conf_ca_chain(&n->conf, &n->cacert, NULL);
////    mbedtls_ssl_conf_rng(&n->conf, mbedtls_ctr_drbg_random, &n->ctr_drbg);
////
////#ifdef MBEDTLS_DEBUG_C
////    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
////    mbedtls_ssl_conf_dbg(&n->conf, mbedtls_debug, NULL);
////#endif
////    if ((ret = mbedtls_ssl_setup(&n->ssl, &n->conf)) != 0)
////    {
////        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
////        goto exit;
////    }
////
////	mbedtls_net_init(&n->server_fd);
////
////	sprintf(portbuf,"%d",port);
////	ESP_LOGI(TAG, "Connecting to %s:%s...", addr, portbuf);
////
////	if ((ret = mbedtls_net_connect(&n->server_fd, addr,
////								  portbuf, MBEDTLS_NET_PROTO_TCP)) != 0)
////	{
////		ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
////		goto exit;
////	}
////
////	ESP_LOGI(TAG, "Connected.");
////
////	ret = mbedtls_net_set_block(&n->server_fd);
////
////	mbedtls_ssl_set_bio(&n->ssl, &n->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
////
////	ESP_LOGD(TAG, "Performing the SSL/TLS handshake...");
////
////	while ((ret = mbedtls_ssl_handshake(&n->ssl)) != 0)
////	{
////		if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
////		{
////			ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
////			goto exit;
////		}
////	}
////
////	ESP_LOGD(TAG, "Verifying peer X.509 certificate...");
////
////	if ((flags = mbedtls_ssl_get_verify_result(&n->ssl)) != 0)
////	{
////		/* In real life, we probably want to close connection if ret != 0 */
////		ESP_LOGW(TAG, "Failed to verify peer certificate!");
////		bzero(n->ws_recvbuf, sizeof(n->ws_recvbuf));
////		mbedtls_x509_crt_verify_info((char *)n->ws_recvbuf, sizeof(n->ws_recvbuf), "  ! ", flags);
////		ESP_LOGW(TAG, "verification info: %s", n->ws_recvbuf);
////	}
////	else {
////		ESP_LOGD(TAG, "Certificate verified.");
////	}
////
////	if (n->websocket) {
////		// ToDo: generate random UUID i.e. Sec-WebSocket-Key use make gen_uuid all
////		sprintf((char *)n->ws_sendbuf,"GET /mqtt HTTP/1.1\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nHost: %s:%d\r\nOrigin: https://%s:%d\r\nSec-WebSocket-Key: %s\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Protocol: mqtt\r\n\r\n",addr,port,addr,port,MBEDTLS_WEBSOCKET_UUID);
////
////		ESP_LOGD(TAG, "req=[%s]",n->ws_sendbuf);
////
////		while((ret = mbedtls_ssl_write(&n->ssl,n->ws_sendbuf,strlen((const char *)n->ws_sendbuf))) <= 0) {
////			if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
////				ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
////				goto exit;
////			}
////		}
////
////		len = ret;
////		ESP_LOGD(TAG, "%d bytes written", len);
////		ESP_LOGD(TAG, "Reading HTTP response...");
////
////		do {
////			bzero(n->ws_recvbuf, sizeof(n->ws_recvbuf));
////			ret = mbedtls_ssl_read(&n->ssl, n->ws_recvbuf, MBEDTLS_WEBSOCKET_RECV_BUF_LEN-1);
////
////			if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
////				continue;
////			}
////
////			if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
////				ESP_LOGD(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
////				ret = 0;
////				break;
////			}
////
////			if(ret < 0) {
////				ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
////				break;
////			}
////
////			if(ret == 0) {
////				ESP_LOGD(TAG, "connection closed");
////				break;
////			}
////
////			len = ret;
////			ESP_LOGD(TAG, "%d bytes read", len);
////#if defined(MBEDTLS_MQTT_DEBUG)
////			/* Print response directly to stdout as it is read */
////			for(int i = 0; i < len; i++) {
////				putchar(n->ws_recvbuf[i]);
////			}
////#endif
////			break;
////		} while(1);
////
////		if (strncmp((const char *)n->ws_recvbuf, "HTTP/1.1 101 Switching Protocols",32)!=0) {
////			goto exit;
////		}
////
////		unsigned char buf[512];
////		unsigned char hash[20];
////		size_t buflen;
////
////		memset(buf,0,sizeof(buf));
////		sprintf((char *)buf,"%s%s",MBEDTLS_WEBSOCKET_UUID,"258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
////		mbedtls_sha1(buf, strlen((const char*)buf), hash);
////		mbedtls_base64_encode(buf, sizeof(buf), &buflen,hash, sizeof(hash));
////#if defined(MBEDTLS_MQTT_DEBUG)
////		printf("client hash %s\n",buf);
////#endif
////
////		if(strstr((char *)n->ws_recvbuf, (char *)buf)==NULL) {
////			ESP_LOGE(TAG, "WebSocket handshake error, Sec-WebSocket-Accept invalid");
////			goto exit;
////		}
////
////
////
////	}
////	retVal = 0;
////
////	return retVal;
////
////exit:
////	NetworkDisconnect(n);
////
////	return retVal;
////}
////
////void TLS_Init(Network* n)
////{
////	n->ws_recv_offset=0;
////	n->ws_recv_len=0;
////	n->mqtt_recv_offset=0;
////	n->mqtt_recv_len=0;
////	n->mqttread=mqtt_mbedtls_read;
////	n->mqttwrite=mqtt_mbedtls_write;
////	n->websocket=0;
////}
//
//void lw_initialise_wifi(void) {
//	tcpip_adapter_init();
//	wifi_event_group = xEventGroupCreate();
//	ESP_ERROR_CHECK(esp_event_loop_init(lw_event_handler, NULL));
//	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//	ESP_ERROR_CHECK (esp_wifi_set_storage(WIFI_STORAGE_RAM) );
//	wifi_config_t wifi_config = { .sta = { .ssid = WIFI_SSID, .password = WIFI_PASS, }, };
//	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...",
//			wifi_config.sta.ssid);
//	ESP_ERROR_CHECK (esp_wifi_set_mode(WIFI_MODE_STA) );
//	ESP_ERROR_CHECK (esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
//	ESP_ERROR_CHECK (esp_wifi_start() );
//}
//
//void lw_mqtt_task(void *pvParameters) {
//	Network network;
//	ESP_ERROR_CHECK(i2c_master_init());
//
//	int ret;
//	char buf[10];
//	uint8_t sensor_data_h, sensor_data_l;
//	uint16_t sensor_data;
//	/* Wait for the callback to set the CONNECTED_BIT in the
//	 event group.
//	 */
//	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
//			false, true, portMAX_DELAY);
//	ESP_LOGI(TAG, "Connected to AP");
//
//	ESP_LOGI(TAG, "Start MQTT Task ...");
//
//	//MQTTClient client;
//	NetworkInit(&network);
//	network.websocket = MQTT_WEBSOCKET;
//
//	ESP_LOGI(TAG, "NetworkConnect %s:%s ...", MQTT_SERVER, MQTT_PORT);
//	NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
//	ESP_LOGI(TAG, "MQTTClientInit  ...");
//
//	esp_mqtt_init(
//					status_callback,
//					message_callback,
//					256,
//					100);
////
////	esp_mqtt_start(
////			MQTT_SERVER,
////			MQTT_PORT,
////			"LW_MQTT",
////			MQTT_USER,
////			MQTT_PASS);
//
//	while (1) {
//
//		ret = i2c_master_read_sensor(I2C_MASTER_NUM, &sensor_data_h,
//				&sensor_data_l);
//		sensor_data = (uint16_t) sensor_data_h << 8 | sensor_data_l;
//		sprintf(buf, "%u", sensor_data);
//
//		if (status) {
//			if (ret == ESP_ERR_TIMEOUT) {
//				ESP_LOGE(TAG, "I2C Timeout");
//			} else if (ret == ESP_OK) {
//				esp_mqtt_publish("device/id1/data", (uint8_t *) buf, strlen(buf) + 1,
//						0, false);
//			} else {
//				ESP_LOGW(TAG, "%s: No ack, sensor not connected. ",
//						esp_err_to_name(ret));
//			}
//		}
//	}
//}
