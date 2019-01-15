/*
 * cert.c
 *
 *  Created on: 27.11.2018
 *      Author: Mario
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

const char *server_root_cert = "-----BEGIN CERTIFICATE-----\n"
		"MIIDfDCCAmSgAwIBAgIJAJ0o535+y4GxMA0GCSqGSIb3DQEBCwUAMFMxETAPBgNV\n"
		"BAMMCDEwLjAuMC4xMQwwCgYDVQQKDANTTUMxDDAKBgNVBAsMA0dUQzEiMCAGCSqG\n"
		"SIb3DQEJARYTbWlvc2dhLm1hcmlvQHNtYy5kZTAeFw0xOTAxMDkwNjU1MTFaFw0z\n"
		"MjAxMDYwNjU1MTFaMFMxETAPBgNVBAMMCDEwLjAuMC4xMQwwCgYDVQQKDANTTUMx\n"
		"DDAKBgNVBAsMA0dUQzEiMCAGCSqGSIb3DQEJARYTbWlvc2dhLm1hcmlvQHNtYy5k\n"
		"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL+zp0oKFRjcizAB8noK\n"
		"cTcKdgGweC5690N6VZ4R8bSLatrkiOLN/G9sE69BiIl7TeUtumVBIjgDBpQdkg95\n"
		"w6E5ztxK5GOqeK2Csy4dVa7NMkI5dixRe/8Zihn/SCf2QDcx/shZB4/G1Jk7xYtl\n"
		"M3Kowclq5U8ZsdXO4otvJRMT7mEXSfmrrW+T/fKheGyTERfLQHhccvCIjxj+ff1F\n"
		"ggN1MhM4KQdf1HHb+i7v6dmr8gljfW/VtWuk6MBNtZZg7f6vSorI8QxcADmqZSYp\n"
		"mizwH4K1iFjFNrQqNdtXyAdN4bkDuC77HlYs5eUeNWAyAF7V/I6dTgQ5oDilu0sc\n"
		"lT8CAwEAAaNTMFEwHQYDVR0OBBYEFJBtM5dSmodMdDWc6hbs8dsjEXEvMB8GA1Ud\n"
		"IwQYMBaAFJBtM5dSmodMdDWc6hbs8dsjEXEvMA8GA1UdEwEB/wQFMAMBAf8wDQYJ\n"
		"KoZIhvcNAQELBQADggEBAIyVPZMdA+qOo3tsZkLRmtdTPLgen6DTRHis66MNCgoL\n"
		"HrE57hFI1yJCxe2zUcApusYLX+C0s8xMgl1+suMqfL93AtajZr/2vPv2V0DOGn8h\n"
		"2CVOWxgawut6EiYQQ+M6UklDG/ko805rL2GZPpl0DeWwzU7mzMSdZbZDPLClLMvz\n"
		"+9vLtX+ubCR9AhF4WDD9oJzYapC+P4NVKlUcPHb7wtw4Mqjsz8wrSmPc/vtlbPkG\n"
		"NuJgsDDFyewAXGTo3qzT0fc5RAza3NrnRX4DtSBnOJuh25nXA7nB19wI/f9e0pqa\n"
		"/1QmI7qL6PHM6RNXaI1dCuiKVZvrhlU6U+iijceh43M=\n"
		"-----END CERTIFICATE-----\n";

