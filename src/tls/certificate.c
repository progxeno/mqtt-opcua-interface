/*
 * certificate.c
 *
 *  Created on: 15.11.2018
 *      Author: Mario
 */


#include "certificate.h"

/* m2mqtt_srv.crt: see
 * https://mcuoneclipse.com/2017/04/14/enable-secure-communication-with-tls-and-the-mosquitto-broker/
 */

#define M2MQTT_CA_CRT_RSA                                                 \
"-----BEGIN CERTIFICATE-----\r\n"                                         \
"MIIDIDCCAggCCQCUTXSPenGUBzANBgkqhkiG9w0BAQsFADBSMQswCQYDVQQGEwJD\r\n"    \
"SDEUMBIGA1UECAwLU3dpdHplcmxhbmQxEDAOBgNVBAcMB0x1Y2VybmUxDTALBgNV\r\n"    \
"BAoMBEhTTFUxDDAKBgNVBAsMA1QmQTAeFw0xNzA0MjMwNjQxMjdaFw0yNzA0MjEw\r\n"    \
"NjQxMjdaMFIxCzAJBgNVBAYTAkNIMRQwEgYDVQQIDAtTd2l0emVybGFuZDEQMA4G\r\n"    \
"A1UEBwwHTHVjZXJuZTENMAsGA1UECgwESFNMVTEMMAoGA1UECwwDVCZBMIIBIjAN\r\n"    \
"BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxGPL7lmqeDkq9Ah3DTieY0xQqV2Z\r\n"    \
"oxSHUUpQ10e+A7bhNbumREdRlQP1mHgaBX0IIiflNMHQ7WYZtrfFzVa09tguzN2o\r\n"    \
"PGhRp0aamKRYsfOa1rTZVy31zzHupKv+NlCi0LXqO1k8Tvvf/BbTa7GYB1hNwxdL\r\n"    \
"c53rRZtxqveHUt5vywA6jpdux6SL9wmzFcspHJ+WOjfp09SOrLFAc9S3HsBwnK9T\r\n"    \
"eDn7hVrR4I/6ne8mRLcCTtL1qLUcGHWc2lAep5hyZBKepSVxC5KaqoHlAOyyMVYJ\r\n"    \
"yDhQhJ0W4ovCKjurK2rpUDIgpjlxnCzNr6xtGXvScRFklF94PiH4x7O4DQIDAQAB\r\n"    \
"MA0GCSqGSIb3DQEBCwUAA4IBAQAFVuA5+QarBpT7U7w66JhrikxDTwfg/6Q7Guuo\r\n"    \
"AeuxpU0ida2UDqy3M31d5e/g906DbxctyeVERPXeDbivRFvJixGj2ry2mbjqHaYe\r\n"    \
"3OYGM2QEhfW3jSkd4IOzrxD5J3BEYLwtz7Yb/7w+CRO34f/5MlYQJWUZiM+IyFh4\r\n"    \
"R94rS3ietTK9AUnFTforc9GPBSjtagQVCdWoYo43+D2z/jDDIigkVhXL1ozJgsx/\r\n"    \
"DwoYYGjCqhommSthyzaQsIu4LyDZ0UZ4uUboTAl6yzmGjuO92TdmZl9wMi91dDfv\r\n"    \
"piwtIz+WkK31VWQYVtY8jIwowTuK6X5IBgodFv04/GYE6sWh\r\n"    \
"-----END CERTIFICATE-----\r\n"

const char mbedtls_m2mqtt_srv_crt[] = M2MQTT_CA_CRT_RSA;
const size_t mbedtls_m2mqtt_srv_crt_len = sizeof(mbedtls_m2mqtt_srv_crt);
