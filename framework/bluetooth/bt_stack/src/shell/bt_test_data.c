/** @file
 * @brief Bluetooth snoop log capture
 *
 * Copyright (c) 2019 Actions Semi Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdlib.h>
#include <string.h>

#define TEST_SCO_MAX_NUM		10
#define TEST_SCO_LEN			60

struct bt_audio_data_len {
	uint16_t len;
	const uint8_t *data;
};

static const uint8_t *test_sco[TEST_SCO_MAX_NUM] = {
	"\xed\xdd\x56\x29\x09\x49\x69\xb5\xdd\x56\x95\x52\x4a\xb5\xd6\x5a\x29\x49\x52\x69\x6d\xb7\xd5\x2a\x29\x4a\xa5\xd5\xb6\xaa\xaa\xa4\x54\x55\x6b\xab\x55\xa5\x28\xa9\xd6\xb6\xad\x55\x8a\x48\x92\xaa\x6d\xf7\xd6\x4a\x24\xa4\xb4\xd6\xed\x56\x95\x24",
	"\x94\x52\xad\xed\x56\x2d\xa5\x28\xa9\x6a\x6d\xd7\xaa\x2a\x29\xa9\xaa\x56\x55\x55\x55\x55\xd5\xaa\xaa\x94\x52\xd5\xb6\x6d\x55\x95\x44\x92\xaa\xda\xb6\xd5\xaa\x94\x94\xb2\xda\xb6\xad\x95\x12\x92\x94\xaa\xb6\x7b\xad\x2a\x89\x24\xa9\xda\xdd\x5d",
	"\xab\x8a\x08\x22\xa9\x75\xdf\x5d\x2b\x22\x88\x54\xdb\xbd\xad\x2a\x12\x52\x6a\xdd\xdd\xaa\x14\x11\x29\x6d\x7b\xd7\xaa\x92\x48\xaa\xd6\x6d\xab\x4a\x92\x28\x55\x6d\x6b\xad\x2a\xa5\xaa\xaa\xaa\xd6\x4a\x49\xa9\x52\xab\xd5\x5a\x6b\x55\x55\x55\xa9",
	"\x55\x55\x95\x92\xa4\x52\x6b\xad\xaa\xaa\xb5\x6d\xdb\x4a\x42\xa4\x6a\xf7\xad\x8a\x20\x48\xd2\xda\xdd\xb6\x4a\x49\xa9\xaa\xaa\x54\x22\x65\xd5\xee\xdd\xda\xd6\xaa\x5a\x6d\x6d\xd7\x4a\x22\x84\x48\x92\x52\x29\xa5\x94\xaa\x5a\xab\xaa\x4a\x84\xa4",
	"\x54\xdb\xdd\x7d\xdf\xdd\xdd\xed\xee\x6e\xab\x24\x22\x08\x22\x12\x49\x22\x29\x49\xa9\x52\x55\xad\x4a\x49\x49\xd5\xf6\xf7\x7d\xef\xb6\xdb\xb6\xb6\xd6\x4a\x49\x12\x89\x88\x44\x22\x49\x92\x52\xa5\xaa\xaa\xaa\x94\x24\x92\x64\xdd\xff\xf7\xef\x6d",
	"\xdb\x6a\xb5\x5a\x2a\x89\x88\x88\x44\x52\x52\x2a\xa5\x52\x55\xd5\x6a\x55\x25\x89\x48\xaa\xf6\xff\xfb\x77\xdb\xda\xaa\x55\x29\x89\x08\x91\x94\x4a\xa9\x54\x4a\x55\x55\x55\x55\x55\x55\xab\xaa\x24\x92\xa4\x76\xff\xf7\xb7\x6b\xad\x56\x55\x29\x22",
	"\x44\x24\x55\x95\x52\x4a\x55\xd5\x5a\x55\x55\xa5\xaa\xd5\xaa\x12\x11\x49\xda\xf7\xff\xde\xad\xd5\x5a\x55\x4a\x84\x90\x28\x55\xa5\xaa\x2a\x55\xd5\xaa\xaa\x4a\x55\x55\x55\xd5\x5a\x95\x84\x88\xa8\x7e\xff\xdf\xad\xd5\xda\xd6\x8a\x08\x82\x24\x95",
	"\x4a\xa5\xaa\xaa\x6a\x55\x55\x55\xa9\x6a\x55\x55\xaa\xb5\x55\x89\x10\x51\xf7\xff\x77\xad\x6a\x6d\xab\x8a\x08\x48\x52\x55\xa5\x54\x55\x6b\xb5\x4a\x29\xa5\xaa\xaa\x5a\x55\x55\x55\xad\x6d\x2b\x09\x08\x51\xdd\xff\x6f\xab\xd5\xb6\xad\x8a\x00\x21",
	"\x55\x55\x95\x54\xb5\xb5\x55\x95\xa4\xa4\xaa\x52\xb5\x52\x55\xad\x56\xdb\x56\x09\x21\x24\xf5\xf7\xdf\x6d\x6d\xdb\x4d\x09\x08\x92\xaa\x94\xa4\x54\xdb\x77\x55\x29\x29\x55\xa5\x24\xa5\xd5\x56\x55\x55\xb5\xdd\x96\x20\x40\x54\xf7\x7f\xd7\xda\xf6",
	"\xde\x2a\x08\x88\x92\x4a\x4a\xaa\xb6\x6d\xad\x56\x55\x29\x89\x48\xaa\x6a\x55\xad\xb5\xd6\xaa\xaa\x6a\x2b\x09\x10\xa9\xfb\xef\xb5\xda\x7d\x6f\x25\x00\x51\xa5\x24\x49\x6a\xf7\x56\x55\x55\x6b\x25\x22\x24\x55\xad\x2a\xb5\x6d\x5b\x4b\xa9\xaa\xad"
};


uint8_t *bt_test_get_sco_data(uint16_t *len)
{
	static uint8_t index;

	if (index >= 10) {
		index = 0;
	}

	*len = TEST_SCO_LEN;
	return (uint8_t *)test_sco[index++];
}

static const uint8_t sbc_data1[] = {
	0x80, 0x60, 0x00, 0x36, 0x00, 0x00, 0xba, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x06, 0x9c, 0xbd, 0x23,
	0xf0, 0xf0, 0x64, 0x22, 0x00, 0x00, 0x41, 0x00,
	0x10, 0x00, 0xbc, 0x29, 0x96, 0x62, 0x74, 0x07,
	0xbc, 0xb6, 0x4d, 0xc7, 0x16, 0x77, 0xad, 0xbe,
	0x1c, 0xee, 0xe5, 0xb7, 0x73, 0xe1, 0x94, 0xb6,
	0xa4, 0x6b, 0xcb, 0x37, 0x10, 0x92, 0x97, 0x99,
	0x1a, 0xd1, 0x6f, 0x10, 0x75, 0x77, 0x38, 0xa5,
	0x72, 0x90, 0x5d, 0x1e, 0x30, 0x2e, 0xc9, 0x38,
	0x55, 0x6f, 0x69, 0x58, 0xee, 0x79, 0xed, 0x35,
	0x1d, 0xd5, 0xc7, 0xd8, 0x6b, 0x2b, 0xc9, 0x37,
	0x55, 0x77, 0x60, 0x65, 0xfa, 0x4e, 0xcb, 0xa3,
	0x9c, 0xbd, 0x23, 0x19, 0xf8, 0x64, 0x31, 0x10,
	0x00, 0x41, 0x10, 0x00, 0x00, 0xa7, 0x5b, 0xa6,
	0x7c, 0x74, 0x71, 0x72, 0xe5, 0x4e, 0xa2, 0x4e,
	0x99, 0xd1, 0xc5, 0x2d, 0xf2, 0xe6, 0x38, 0xc9,
	0x43, 0x5c, 0x27, 0x3e, 0xd6, 0x2a, 0xd6, 0xde,
	0xa0, 0xf1, 0x46, 0xda, 0xab, 0x9a, 0x2d, 0x64,
	0x59, 0x6b, 0x26, 0x29, 0x8e, 0xb5, 0x91, 0x3a,
	0x0a, 0x0d, 0x57, 0x95, 0xf8, 0xcc, 0x95, 0xf0,
	0xd7, 0x3b, 0xd8, 0x3b, 0x9a, 0x37, 0x64, 0xc7,
	0xaa, 0xe6, 0xf5, 0x1e, 0xed, 0x70, 0x9f, 0x2b,
	0x61, 0x2e, 0xe3, 0x9c, 0xbd, 0x23, 0x98, 0xf0,
	0x64, 0x23, 0x00, 0x00, 0x42, 0x01, 0x10, 0x00,
	0xe9, 0x4c, 0x34, 0xd2, 0xbc, 0x30, 0x96, 0xa9,
	0x57, 0x26, 0x2e, 0xe5, 0xba, 0xe4, 0xb1, 0x9c,
	0xf8, 0x5b, 0xd7, 0xbc, 0x70, 0xcb, 0x32, 0xe6,
	0x6c, 0xe1, 0x69, 0x1c, 0xed, 0xcb, 0xac, 0x0c,
	0x19, 0x36, 0x75, 0x47, 0x72, 0xb8, 0x0e, 0xc3,
	0x72, 0x79, 0x71, 0x54, 0x39, 0x48, 0xf2, 0xd3,
	0x14, 0x2c, 0xd5, 0x19, 0x53, 0x5c, 0x86, 0x8e,
	0x42, 0xc3, 0x57, 0x70, 0xd5, 0xa0, 0x96, 0xcd,
	0xd4, 0x34, 0x95, 0x15, 0xbb, 0x65, 0x9c, 0xbd,
	0x23, 0xab, 0xf8, 0x64, 0x32, 0x00, 0x00, 0x32,
	0x10, 0x00, 0x00, 0xbb, 0x35, 0x95, 0xd7, 0x78,
	0xaa, 0x33, 0x9a, 0xaf, 0x2d, 0x36, 0x70, 0xd6,
	0x5c, 0x98, 0xee, 0xbc, 0xbb, 0x33, 0x9d, 0xb3,
	0x77, 0x78, 0x94, 0x3e, 0x8c, 0xa3, 0x0d, 0x7b,
	0xa9, 0xdb, 0xfe, 0x51, 0x16, 0xba, 0x8c, 0x35,
	0xcf, 0x47, 0x6e, 0x29, 0x3b, 0x64, 0xcd, 0x72,
	0xe7, 0x9d, 0xe1, 0xae, 0x9b, 0x13, 0xc4, 0x27,
	0x9e, 0x25, 0x6b, 0x34, 0xfe, 0x66, 0x4a, 0xf0,
	0x21, 0xe4, 0x7d, 0x37, 0x5d, 0xbd, 0xf0, 0xc6,
	0xbb, 0x9c, 0xbd, 0x23, 0xa4, 0xf0, 0x64, 0x32,
	0x00, 0x00, 0x32, 0x10, 0x00, 0x00, 0xb0, 0xb6,
	0x25, 0x07, 0x79, 0x47, 0x43, 0xa5, 0x0e, 0xf8,
	0xe9, 0x64, 0x9d, 0xd7, 0x93, 0x10, 0x4d, 0x39,
	0x2b, 0xa1, 0xca, 0x86, 0xe5, 0x94, 0x3c, 0xaa,
	0xdf, 0x4e, 0x65, 0xce, 0x1b, 0xb5, 0xaf, 0x34,
	0x3c, 0x5e, 0xbd, 0xe5, 0xd7, 0x6f, 0x97, 0xba,
	0x3e, 0xed, 0x86, 0xf6, 0x57, 0x1e, 0x21, 0x5c,
	0xd2, 0xac, 0x43, 0x1c, 0x9a, 0x22, 0x56, 0x73,
	0x62, 0xbe, 0x31, 0x15, 0xe8, 0x23, 0xae, 0xea,
	0xef, 0x09, 0x91, 0x94, 0x9c, 0xbd, 0x23, 0x7b,
	0xf0, 0x74, 0x21, 0x00, 0x00, 0x32, 0x00, 0x00,
	0x00, 0x82, 0x66, 0x79, 0x30, 0xd0, 0x45, 0x8f,
	0x5f, 0x46, 0x0b, 0x42, 0x2b, 0x96, 0x47, 0xda,
	0x43, 0x82, 0xf8, 0xed, 0x48, 0x8b, 0x17, 0x25,
	0xad, 0x90, 0x41, 0xe1, 0xad, 0x35, 0x9a, 0x93,
	0xf1, 0xa6, 0x28, 0x32, 0x7e, 0xcc, 0xe5, 0x1d,
	0x8d, 0x87, 0x5b, 0x9c, 0xbd, 0xc5, 0x2c, 0xa4,
	0x47, 0x37, 0x5d, 0x79, 0x86, 0xe5, 0x84, 0x95,
	0x4c, 0x9c, 0x8f, 0x76, 0x8a, 0x14, 0x0e, 0x2e,
	0xca, 0x63, 0x82, 0x03, 0x7b, 0xdb, 0xc4
};

static const uint8_t sbc_data2[] = {
	0x80, 0x60, 0x00, 0x37, 0x00, 0x00, 0xbd, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0x46, 0xf8, 0x74, 0x32, 0x00, 0x00, 0x42, 0x11,
	0x00, 0x00, 0x43, 0x86, 0x19, 0x0c, 0x2a, 0x45,
	0x8d, 0x1b, 0x51, 0xb1, 0xd3, 0x20, 0xba, 0xb7,
	0x42, 0x63, 0x87, 0x58, 0x27, 0x44, 0x6b, 0x0a,
	0xf2, 0xf9, 0x6c, 0x5d, 0x5e, 0x15, 0x0d, 0x34,
	0xab, 0x83, 0x9d, 0xaf, 0x85, 0x60, 0xa3, 0xb9,
	0xe8, 0xac, 0x0e, 0x65, 0x92, 0x15, 0x55, 0x8e,
	0xf0, 0xba, 0xa1, 0x49, 0xe5, 0xe6, 0x54, 0x56,
	0xb4, 0xae, 0xea, 0x62, 0xf6, 0x54, 0x9d, 0x4b,
	0x5c, 0xce, 0xdc, 0x29, 0x84, 0x1a, 0x64, 0x65,
	0x9c, 0xbd, 0x23, 0xc2, 0xf0, 0x74, 0x31, 0x00,
	0x00, 0x32, 0x00, 0x00, 0x00, 0x3d, 0xb4, 0x59,
	0xb6, 0x4a, 0x12, 0x08, 0x63, 0xed, 0x57, 0x4a,
	0x9b, 0x34, 0x3c, 0xf3, 0x45, 0x60, 0x68, 0x3a,
	0xa9, 0x6c, 0x65, 0x27, 0xad, 0x52, 0x3b, 0x28,
	0x69, 0xa5, 0xc5, 0x9d, 0x02, 0x66, 0x35, 0x6d,
	0xa5, 0xd6, 0xd8, 0xa5, 0x12, 0xd5, 0x5b, 0x1c,
	0xd6, 0x4e, 0xd3, 0x9c, 0xf9, 0xcd, 0x68, 0x61,
	0xea, 0xb8, 0xba, 0xee, 0xdd, 0xa7, 0x2a, 0x9a,
	0x52, 0x1b, 0x62, 0xee, 0x3d, 0x1d, 0x7b, 0xb6,
	0x18, 0x24, 0x90, 0x9c, 0xbd, 0x23, 0x47, 0xf0,
	0x64, 0x32, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00,
	0x5f, 0x3b, 0x10, 0xd8, 0xe9, 0x16, 0x59, 0x1e,
	0xa5, 0x95, 0xae, 0x64, 0xd9, 0x33, 0x27, 0xa2,
	0xa4, 0x38, 0x7a, 0x13, 0x0f, 0x99, 0x50, 0x6e,
	0x69, 0x69, 0xe5, 0x4f, 0x4b, 0x27, 0x3e, 0x21,
	0xe8, 0x83, 0x57, 0xbb, 0xd3, 0x30, 0x7e, 0xba,
	0x54, 0x1e, 0x15, 0xef, 0x62, 0xdb, 0xba, 0xb3,
	0xe5, 0x6b, 0x86, 0x56, 0x4d, 0x03, 0x6e, 0xcf,
	0x93, 0x7a, 0x76, 0x22, 0x31, 0xab, 0xd3, 0xb5,
	0x37, 0xb5, 0xe1, 0x86, 0x75, 0xc8, 0x9c, 0xbd,
	0x23, 0xa0, 0xf0, 0x64, 0x31, 0x00, 0x00, 0x42,
	0x00, 0x00, 0x00, 0x7e, 0xc8, 0xe9, 0xbd, 0x8d,
	0x67, 0x99, 0xbd, 0xc5, 0x28, 0xf2, 0xc8, 0x16,
	0xa3, 0x60, 0x49, 0x35, 0x05, 0x10, 0x24, 0x27,
	0x7c, 0xa6, 0x8d, 0x44, 0x0b, 0xa3, 0x96, 0xa5,
	0x77, 0x3c, 0xa1, 0x94, 0x6c, 0x91, 0xc7, 0xd6,
	0xb1, 0x21, 0xbb, 0xf3, 0x56, 0x1a, 0xd7, 0x6f,
	0x23, 0x32, 0xe6, 0xf5, 0xae, 0x76, 0x46, 0x8d,
	0x72, 0xcc, 0xcc, 0xa9, 0xb3, 0x69, 0xdb, 0x84,
	0x37, 0xf3, 0x3a, 0xa6, 0x6d, 0xc5, 0x37, 0xb3,
	0xd5, 0x9c, 0xbd, 0x23, 0x6e, 0xf0, 0x64, 0x33,
	0x00, 0x00, 0x32, 0x01, 0x00, 0x00, 0xbf, 0xca,
	0x21, 0xe5, 0x71, 0x06, 0x3b, 0xb4, 0xed, 0x9d,
	0x08, 0x77, 0x61, 0xad, 0xe0, 0xef, 0x2a, 0xb4,
	0x7a, 0xea, 0x29, 0x94, 0xd4, 0xc5, 0xbc, 0xa6,
	0xd5, 0x18, 0xa2, 0x6c, 0xab, 0x3d, 0x92, 0xe7,
	0x53, 0x78, 0xaa, 0x9c, 0x78, 0x6e, 0xe7, 0xd3,
	0x95, 0x0e, 0xba, 0xf9, 0x82, 0xd9, 0x4a, 0xeb,
	0x0e, 0x82, 0x3a, 0x32, 0x5d, 0xd8, 0xa7, 0x31,
	0x64, 0x3c, 0x10, 0xe0, 0x34, 0x87, 0xa5, 0x9c,
	0x89, 0xcc, 0xdc, 0xc3, 0x9c, 0xbd, 0x23, 0xf4,
	0xf8, 0x65, 0x32, 0x00, 0x00, 0x32, 0x10, 0x00,
	0x00, 0x85, 0x3d, 0xdb, 0x95, 0x71, 0x78, 0xb5,
	0xca, 0xee, 0x82, 0xe7, 0x95, 0xe5, 0xb8, 0x22,
	0xe9, 0x0a, 0xb6, 0xe3, 0x99, 0xe6, 0x76, 0xa9,
	0x7b, 0xc3, 0xb0, 0xdc, 0xcf, 0x57, 0x86, 0x64,
	0x30, 0xe4, 0xf4, 0x6d, 0x8b, 0x39, 0xc6, 0x28,
	0x77, 0x49, 0x23, 0xba, 0xd2, 0xac, 0xe4, 0x76,
	0x5e, 0xe4, 0x22, 0xb0, 0x83, 0x4c, 0x02, 0xd6,
	0x51, 0x99, 0x71, 0x83, 0xbb, 0x6e, 0xf1, 0x92,
	0x78, 0x6d, 0x5d, 0x4d, 0xab, 0x08, 0xbc, 0x9c,
	0xbd, 0x23, 0x8a, 0xf0, 0x64, 0x22, 0x00, 0x00,
	0x32, 0x01, 0x00, 0x00, 0xa9, 0x36, 0x1d, 0xc7,
	0x73, 0xc8, 0x4b, 0x57, 0x2d, 0xe6, 0xb9, 0xb9,
	0xa1, 0x49, 0xa3, 0x32, 0xcb, 0x36, 0x4b, 0x59,
	0xd8, 0x76, 0x9b, 0x8c, 0x43, 0xb0, 0xcd, 0x4e,
	0x98, 0x8d, 0xd9, 0x3d, 0xb1, 0x13, 0x33, 0x44,
	0xe7, 0x72, 0xf2, 0x44, 0x8e, 0x40, 0x68, 0x75,
	0x5a, 0xfa, 0x2a, 0x5a, 0x39, 0x95, 0x4e, 0xc3,
	0xb8, 0x2c, 0xad, 0x4e, 0x99, 0x84, 0x84, 0x49,
	0x2a, 0xeb, 0xf5, 0x78, 0x1d, 0x1d, 0xfd, 0x4c,
	0xe2, 0xcb
};

static const uint8_t sbc_data3[] = {
	0x80, 0x60, 0x00, 0x38, 0x00, 0x00, 0xc0, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0xbf, 0xf8, 0x75, 0x33, 0x00, 0x00, 0x32, 0x11,
	0x00, 0x00, 0x95, 0x73, 0xb1, 0x21, 0xb1, 0xd1,
	0x76, 0x6b, 0xb6, 0x51, 0xea, 0xe4, 0x58, 0xbf,
	0x3d, 0xde, 0xca, 0xd7, 0xc8, 0xac, 0xb6, 0x62,
	0xe4, 0xd7, 0x91, 0x43, 0x52, 0xe2, 0xce, 0xc6,
	0x6a, 0x23, 0x5d, 0xed, 0x0d, 0x39, 0x7b, 0x3b,
	0x5d, 0xa9, 0x90, 0x37, 0x1c, 0xd5, 0x40, 0xe9,
	0x5d, 0xc8, 0x2a, 0xb5, 0xca, 0xb2, 0xe6, 0xe8,
	0xa2, 0x42, 0x22, 0xc2, 0xe5, 0x65, 0xb4, 0x60,
	0x20, 0xcc, 0x76, 0x8c, 0x12, 0xd9, 0x8b, 0x4d,
	0x9c, 0xbd, 0x23, 0x3d, 0xf0, 0x74, 0x32, 0x00,
	0x00, 0x32, 0x10, 0x00, 0x00, 0x80, 0xc2, 0x20,
	0xc8, 0x50, 0x7a, 0x3b, 0xaa, 0x8d, 0xd6, 0xe6,
	0x86, 0xd9, 0xb7, 0xdb, 0x31, 0x3b, 0xa5, 0xa4,
	0x1d, 0x36, 0x96, 0x8f, 0x6c, 0x4e, 0xee, 0x96,
	0x12, 0x8a, 0xbd, 0x51, 0xd9, 0x8f, 0x15, 0x43,
	0x33, 0x41, 0xda, 0xe8, 0x64, 0x87, 0xb3, 0xe2,
	0xec, 0x90, 0xb9, 0xaa, 0xa5, 0x1c, 0x27, 0x95,
	0x42, 0x13, 0x67, 0x19, 0x33, 0x26, 0xb3, 0x8c,
	0x86, 0xac, 0x98, 0x69, 0x94, 0xa9, 0xd3, 0x5a,
	0x30, 0xae, 0xca, 0x9c, 0xbd, 0x23, 0xaf, 0xf0,
	0x55, 0x23, 0x00, 0x00, 0x22, 0x01, 0x10, 0x00,
	0x86, 0xe9, 0x83, 0x5d, 0xab, 0x66, 0xae, 0xba,
	0x36, 0x8b, 0x59, 0xec, 0x96, 0xd3, 0x6c, 0x3d,
	0xf2, 0xdc, 0x30, 0x47, 0xa5, 0x9b, 0xc1, 0xea,
	0xf6, 0x3b, 0x65, 0xbd, 0x9b, 0xc5, 0x8e, 0x88,
	0xac, 0x6f, 0x09, 0xd2, 0xd7, 0x86, 0xa1, 0x2f,
	0xa4, 0xce, 0xb2, 0xb5, 0x9b, 0x61, 0xe3, 0x86,
	0x91, 0x8c, 0x3c, 0x8a, 0xd6, 0x72, 0x07, 0x81,
	0x5c, 0x7c, 0xf3, 0x72, 0x71, 0x92, 0x2e, 0x17,
	0x2c, 0x8c, 0xe7, 0xb2, 0xee, 0xd1, 0x9c, 0xbd,
	0x23, 0x05, 0xf0, 0x64, 0x22, 0x00, 0x00, 0x32,
	0x00, 0x00, 0x00, 0x7b, 0x3c, 0xaa, 0xf9, 0xf8,
	0x79, 0xa0, 0xd0, 0x37, 0x08, 0x8b, 0xb1, 0x8a,
	0x6a, 0x1c, 0x97, 0x31, 0x4d, 0x24, 0xee, 0xc4,
	0x51, 0x7b, 0x8e, 0x58, 0x0b, 0xb6, 0x4e, 0xe4,
	0x09, 0x86, 0x09, 0xe6, 0x65, 0xee, 0xab, 0xbb,
	0x4b, 0x12, 0x39, 0x97, 0xa2, 0x9e, 0xba, 0x34,
	0xe3, 0xd3, 0xb8, 0xb9, 0x20, 0x75, 0x94, 0xf3,
	0xeb, 0xab, 0x18, 0x1c, 0xa8, 0x5a, 0xa6, 0xa5,
	0x9e, 0x31, 0x53, 0xb8, 0x5b, 0x37, 0xc1, 0x97,
	0x61, 0x9c, 0xbd, 0x23, 0x69, 0xf0, 0x64, 0x12,
	0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x9f, 0xd6,
	0xe5, 0x98, 0x72, 0x45, 0x9d, 0x0f, 0xde, 0xab,
	0x2b, 0xac, 0xdb, 0xec, 0xd7, 0x57, 0x03, 0x7c,
	0xa6, 0x4c, 0xe2, 0x03, 0xc0, 0x69, 0xe2, 0x5e,
	0x30, 0x69, 0xa4, 0x49, 0xde, 0x42, 0x50, 0x81,
	0xb6, 0xc0, 0xbc, 0xd2, 0x75, 0x11, 0x46, 0x58,
	0xb5, 0x22, 0xad, 0x4a, 0xba, 0xd9, 0xd3, 0x16,
	0x99, 0xc4, 0x98, 0xd5, 0x11, 0x06, 0x4e, 0xeb,
	0x67, 0x60, 0xea, 0x14, 0xf2, 0xcc, 0xa1, 0x8c,
	0x13, 0x05, 0x4a, 0xf3, 0x9c, 0xbd, 0x23, 0xa4,
	0xf0, 0x64, 0x32, 0x00, 0x00, 0x32, 0x10, 0x00,
	0x00, 0x70, 0x12, 0xf9, 0x0b, 0x32, 0x92, 0xcc,
	0x41, 0xaa, 0xce, 0xb8, 0x5a, 0x61, 0x48, 0x1f,
	0x0e, 0xb4, 0x2c, 0x3c, 0x61, 0x06, 0x75, 0x27,
	0x5c, 0x2a, 0x92, 0xa5, 0xeb, 0x78, 0x66, 0x1c,
	0x3e, 0x4e, 0xea, 0x33, 0x7c, 0xce, 0x19, 0x36,
	0x53, 0xb7, 0x44, 0x38, 0xc9, 0xe3, 0x17, 0x78,
	0xd9, 0xb6, 0x61, 0x09, 0xa4, 0x29, 0x33, 0x5d,
	0xb3, 0x86, 0xdb, 0x93, 0xbc, 0x4c, 0xdb, 0x0e,
	0x66, 0x69, 0xd9, 0xdd, 0xed, 0x15, 0x43, 0x9c,
	0xbd, 0x23, 0x44, 0xf0, 0x64, 0x22, 0x00, 0x00,
	0x41, 0x00, 0x00, 0x00, 0x59, 0x37, 0x11, 0x1d,
	0x8d, 0xc5, 0x95, 0x61, 0x91, 0x7b, 0xb8, 0x6d,
	0x70, 0xca, 0xed, 0x65, 0xd0, 0x18, 0x3b, 0x2b,
	0x15, 0x4d, 0x60, 0xa1, 0x5a, 0x41, 0xf6, 0xa9,
	0xb2, 0x45, 0xc6, 0xd7, 0x15, 0x67, 0xb3, 0xf4,
	0xbf, 0x0e, 0xc1, 0x19, 0x25, 0x59, 0x97, 0xd7,
	0x61, 0x0a, 0xba, 0xd6, 0x66, 0x1b, 0x68, 0x5b,
	0xa9, 0xa2, 0xe8, 0xcf, 0xef, 0x72, 0xad, 0x9a,
	0x68, 0xe5, 0x6a, 0xc3, 0xc0, 0xdb, 0x46, 0x68,
	0x96, 0xc7
};

static const uint8_t sbc_data4[] = {
	0x80, 0x60, 0x00, 0x39, 0x00, 0x00, 0xc4, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0xeb, 0xf0, 0x64, 0x32, 0x00, 0x00, 0x42, 0x10,
	0x10, 0x00, 0x2d, 0x5c, 0xb8, 0x90, 0xa1, 0xb1,
	0x75, 0x1d, 0x11, 0x0e, 0xd2, 0xde, 0xba, 0x9a,
	0xa3, 0x2b, 0x6a, 0x16, 0x43, 0x4e, 0x8d, 0x6b,
	0x04, 0xa8, 0x91, 0xe9, 0x5b, 0x5f, 0x0d, 0xf4,
	0x35, 0x6c, 0xa0, 0xb0, 0x66, 0x96, 0x5c, 0x25,
	0x50, 0xd7, 0x2b, 0x74, 0x72, 0x1a, 0xb6, 0x2e,
	0xd2, 0xba, 0xcf, 0x4d, 0xdb, 0x46, 0x5b, 0x57,
	0xbb, 0x8c, 0x8b, 0x01, 0x18, 0x72, 0xdd, 0x5c,
	0x5f, 0x06, 0xac, 0x2c, 0x13, 0x5d, 0x4c, 0x75,
	0x9c, 0xbd, 0x23, 0x79, 0xf8, 0x64, 0x33, 0x00,
	0x00, 0x32, 0x11, 0x00, 0x00, 0x57, 0x93, 0xb7,
	0x95, 0xaa, 0xac, 0x66, 0xdb, 0xb4, 0x6e, 0x2c,
	0xe6, 0x76, 0x99, 0xb1, 0xe3, 0xd4, 0xd3, 0x65,
	0x22, 0x59, 0x6a, 0x53, 0xa3, 0xae, 0x25, 0x1a,
	0xae, 0xb2, 0x73, 0x8a, 0x83, 0x99, 0xcc, 0xd1,
	0x72, 0x9b, 0x39, 0x1e, 0x2f, 0x88, 0x66, 0xec,
	0x35, 0x97, 0x0c, 0x8e, 0x58, 0xbc, 0xb9, 0x98,
	0xee, 0xd4, 0xb5, 0x33, 0x39, 0xda, 0xcd, 0x07,
	0x6e, 0xd3, 0x59, 0x5a, 0xf1, 0xf7, 0x6a, 0x23,
	0x19, 0xce, 0xf1, 0x9c, 0xbd, 0x23, 0x3d, 0xf0,
	0x74, 0x32, 0x00, 0x00, 0x32, 0x10, 0x00, 0x00,
	0x72, 0xd1, 0xd2, 0xe6, 0x6c, 0x24, 0xbb, 0x5d,
	0x0d, 0x55, 0x38, 0x97, 0x1d, 0x9d, 0x59, 0x0e,
	0xd3, 0xb2, 0x84, 0x5d, 0xdc, 0x56, 0x7a, 0xb4,
	0x9c, 0xec, 0x8b, 0x47, 0xd8, 0xbe, 0x81, 0xf0,
	0xf3, 0xdf, 0x5a, 0x45, 0xa2, 0x63, 0xcb, 0x47,
	0x68, 0x4c, 0xee, 0xe9, 0x8d, 0x18, 0x6b, 0x59,
	0x2e, 0x19, 0x0b, 0x2b, 0xb7, 0xe3, 0x21, 0x9d,
	0x96, 0xf2, 0x83, 0xc4, 0x6e, 0xdc, 0x33, 0x76,
	0x8d, 0x9c, 0x29, 0xef, 0x0e, 0xab, 0x9c, 0xbd,
	0x23, 0xeb, 0xf0, 0x64, 0x32, 0x00, 0x00, 0x42,
	0x10, 0x10, 0x00, 0x70, 0x83, 0xc7, 0xd0, 0xaf,
	0x4f, 0x73, 0x45, 0xd6, 0x55, 0xac, 0xad, 0xc2,
	0xb8, 0x4d, 0x9d, 0x45, 0x56, 0xd6, 0x33, 0x9a,
	0xea, 0x5d, 0x06, 0x53, 0x21, 0x54, 0x14, 0xf2,
	0x75, 0x2b, 0x2a, 0x8d, 0x4e, 0x69, 0x65, 0xd1,
	0xea, 0x89, 0x37, 0x18, 0x5b, 0x44, 0xe6, 0x75,
	0xca, 0xe5, 0x33, 0x5f, 0x51, 0x5c, 0x07, 0x6c,
	0xb3, 0xb2, 0x70, 0xeb, 0x89, 0x96, 0x49, 0x59,
	0xb9, 0x9a, 0xcc, 0xd3, 0x2d, 0x5b, 0x1d, 0xa0,
	0x75, 0x9c, 0xbd, 0x23, 0x11, 0xf8, 0x64, 0x32,
	0x00, 0x00, 0x31, 0x10, 0x00, 0x00, 0xc2, 0x3d,
	0xd8, 0xdd, 0x77, 0xa5, 0xbc, 0xa4, 0x8e, 0x18,
	0xe6, 0x67, 0x6e, 0x4f, 0xa9, 0x09, 0x21, 0x38,
	0x8a, 0x5d, 0xde, 0xb7, 0x11, 0xa4, 0x3a, 0xa8,
	0xda, 0x6c, 0x88, 0x5e, 0x9a, 0xc2, 0x4f, 0x0a,
	0x4a, 0x7f, 0xde, 0x25, 0x51, 0x4d, 0x45, 0x63,
	0xc1, 0xa6, 0x74, 0x89, 0xba, 0x79, 0x61, 0x91,
	0x2b, 0x1b, 0x2a, 0x9b, 0xe5, 0xa4, 0x35, 0xc1,
	0x8c, 0x35, 0x16, 0xec, 0xac, 0x85, 0x66, 0xde,
	0x69, 0xb0, 0xe3, 0xa3, 0x9c, 0xbd, 0x23, 0x21,
	0xf0, 0x64, 0x33, 0x00, 0x00, 0x42, 0x01, 0x10,
	0x00, 0xa6, 0x83, 0xb4, 0x8e, 0xb2, 0xb3, 0x76,
	0x99, 0x56, 0xfe, 0x2e, 0xd4, 0x2a, 0xcf, 0x46,
	0x1b, 0x57, 0x59, 0xe7, 0x3a, 0x76, 0xeb, 0x68,
	0xf7, 0x6e, 0x19, 0x62, 0xe6, 0xed, 0xf2, 0xac,
	0x53, 0x19, 0xbf, 0x65, 0x58, 0x8b, 0xb4, 0xf0,
	0xae, 0x8d, 0x76, 0x72, 0x15, 0xf5, 0xe8, 0x99,
	0x2a, 0xb7, 0xe8, 0xec, 0x63, 0x0b, 0x6c, 0xab,
	0x90, 0x6d, 0x1e, 0xc5, 0x72, 0x99, 0xab, 0xa8,
	0xae, 0x63, 0xae, 0x32, 0x99, 0x42, 0x76, 0x9c,
	0xbd, 0x23, 0xc2, 0xf0, 0x64, 0x33, 0x00, 0x00,
	0x32, 0x11, 0x10, 0x00, 0xc1, 0xba, 0x6c, 0x32,
	0xdb, 0xec, 0x6c, 0xb7, 0x56, 0x91, 0xad, 0x83,
	0x0a, 0xd3, 0xb5, 0xb3, 0xe5, 0x59, 0x46, 0x37,
	0x1c, 0x2a, 0xd8, 0xe4, 0xdc, 0x75, 0x64, 0x64,
	0xd2, 0xce, 0xab, 0x9a, 0x1b, 0x9e, 0x55, 0x61,
	0xa3, 0x77, 0xc2, 0xa6, 0x6b, 0x6d, 0xe7, 0x54,
	0x3e, 0x2e, 0x36, 0xca, 0x9b, 0xd2, 0x29, 0xdd,
	0x53, 0x13, 0xdb, 0xa4, 0x84, 0xe0, 0x89, 0x3b,
	0xc5, 0x69, 0xd1, 0x1f, 0x32, 0x34, 0x93, 0xe3,
	0xba, 0x16
};

static const uint8_t sbc_data5[] = {
	0x80, 0x60, 0x00, 0x3a, 0x00, 0x00, 0xc7, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0xcb, 0xf0, 0x64, 0x22, 0x00, 0x00, 0x41, 0x01,
	0x00, 0x00, 0xd7, 0x46, 0xd2, 0x82, 0x96, 0x16,
	0x4b, 0x51, 0x8f, 0x90, 0xe9, 0x86, 0x91, 0xeb,
	0xa1, 0x0c, 0x71, 0xbc, 0xd4, 0xa1, 0xd6, 0x97,
	0xd1, 0x84, 0xc4, 0x74, 0xf0, 0x50, 0x88, 0x88,
	0x9d, 0xa1, 0xf0, 0x71, 0x24, 0xb3, 0xc1, 0xe2,
	0x5a, 0x70, 0x59, 0x34, 0x4c, 0x2e, 0x16, 0xc5,
	0x68, 0x0d, 0xa6, 0xe0, 0xd0, 0xde, 0x36, 0x13,
	0x21, 0xd7, 0x46, 0xbc, 0x7b, 0xd3, 0xd4, 0x91,
	0x18, 0x0e, 0xa0, 0x0c, 0x5a, 0xea, 0xb3, 0xc4,
	0x9c, 0xbd, 0x23, 0xf2, 0xf8, 0x64, 0x32, 0x00,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x81, 0xb2, 0xcb,
	0x0c, 0xd3, 0xda, 0x15, 0x29, 0x5a, 0xae, 0xaa,
	0xad, 0x24, 0xcb, 0xaf, 0x66, 0x6f, 0x4a, 0xf3,
	0x0c, 0xe7, 0xf0, 0xeb, 0x69, 0x9d, 0x32, 0x60,
	0xef, 0x34, 0x2e, 0xdc, 0x41, 0xc6, 0x87, 0xa8,
	0x74, 0xbc, 0xab, 0x21, 0x13, 0x99, 0x9d, 0xa5,
	0x52, 0x1e, 0x93, 0xd4, 0xb5, 0xba, 0x6a, 0x87,
	0xa0, 0x55, 0x22, 0xb0, 0xf0, 0x0e, 0x66, 0x91,
	0x9d, 0x18, 0x94, 0x12, 0xb3, 0x2e, 0x41, 0xc4,
	0x5b, 0xb9, 0x0c, 0x9c, 0xbd, 0x23, 0x7b, 0xf0,
	0x74, 0x21, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
	0x6b, 0x9f, 0x85, 0x77, 0x10, 0x35, 0x2a, 0x27,
	0x45, 0x9c, 0xf4, 0xcd, 0xe7, 0xbf, 0xe5, 0xb9,
	0xac, 0x96, 0xca, 0x94, 0x71, 0xe3, 0x0a, 0x7a,
	0xad, 0x5c, 0x20, 0x90, 0xc5, 0x9d, 0x73, 0x96,
	0x68, 0x31, 0xcb, 0x7c, 0x41, 0x26, 0x9d, 0xad,
	0x68, 0x6a, 0xd0, 0xc1, 0xb0, 0xd5, 0x91, 0xd9,
	0xbb, 0xdf, 0x9d, 0x5a, 0xf5, 0xd4, 0x90, 0x98,
	0x9e, 0xb7, 0x6d, 0xaa, 0xbc, 0x8f, 0x91, 0xca,
	0x3d, 0x89, 0xed, 0x9a, 0x96, 0xf5, 0x9c, 0xbd,
	0x23, 0xd2, 0xf8, 0x74, 0x22, 0x00, 0x00, 0x41,
	0x00, 0x00, 0x00, 0x3c, 0x57, 0x17, 0x2a, 0x05,
	0xed, 0x46, 0x26, 0x09, 0x4d, 0x83, 0x94, 0xf1,
	0x28, 0x57, 0x76, 0xa7, 0x95, 0x6d, 0x4c, 0x94,
	0x92, 0xc2, 0x59, 0x5a, 0x4e, 0x54, 0xf9, 0x23,
	0xbd, 0x1b, 0x5d, 0x75, 0x85, 0x1a, 0x58, 0xb2,
	0xcc, 0x85, 0x0c, 0x47, 0xd5, 0xda, 0xb1, 0xa0,
	0xd2, 0xbc, 0x9b, 0x30, 0x1f, 0x45, 0xa3, 0x56,
	0xf5, 0x09, 0x36, 0x08, 0xc6, 0x41, 0xab, 0x02,
	0x57, 0x14, 0xc3, 0xd6, 0xa2, 0xc9, 0x59, 0x89,
	0xb8, 0x9c, 0xbd, 0x23, 0x96, 0xf0, 0x64, 0x23,
	0x00, 0x00, 0x41, 0x01, 0x00, 0x00, 0x29, 0xc5,
	0xda, 0x38, 0x6a, 0xc7, 0xa3, 0x4b, 0x91, 0x20,
	0x35, 0xb9, 0x78, 0xb5, 0x13, 0x29, 0x62, 0xdb,
	0x33, 0x59, 0x69, 0xc9, 0x4c, 0x72, 0x34, 0xa3,
	0x39, 0xb2, 0x76, 0x8e, 0xa6, 0x05, 0x2e, 0xf2,
	0x9b, 0xd3, 0x42, 0x5d, 0x83, 0x78, 0xa8, 0x4b,
	0x9c, 0x4e, 0x69, 0x17, 0x74, 0x0d, 0xd5, 0x9f,
	0x2e, 0xc8, 0xb9, 0x7c, 0x29, 0xd9, 0x47, 0x34,
	0x65, 0xc3, 0x48, 0xec, 0x91, 0xc8, 0x7e, 0x9c,
	0x2a, 0x12, 0xd2, 0x9b, 0x9c, 0xbd, 0x23, 0x76,
	0xf0, 0x64, 0x33, 0x00, 0x00, 0x32, 0x11, 0x00,
	0x00, 0x86, 0x73, 0xb8, 0x61, 0xab, 0xb1, 0x86,
	0x83, 0xb5, 0x99, 0xf6, 0xc0, 0x92, 0xbf, 0xdf,
	0x20, 0xe6, 0x56, 0x8b, 0xc9, 0x69, 0x55, 0x51,
	0x49, 0x52, 0xb2, 0x6b, 0xd1, 0x2a, 0x99, 0x2d,
	0x64, 0xe5, 0xdd, 0x29, 0xd0, 0x54, 0x4c, 0x21,
	0x35, 0x76, 0x76, 0xfa, 0xb7, 0x8d, 0x6c, 0xcc,
	0x96, 0xdd, 0xb1, 0x98, 0x30, 0xdb, 0xb8, 0xb3,
	0x26, 0x1b, 0x6a, 0xe6, 0x67, 0x3b, 0x66, 0x24,
	0xec, 0xd4, 0x6d, 0x6c, 0xd6, 0x0c, 0xf1, 0x9c,
	0xbd, 0x23, 0x5f, 0xf0, 0x64, 0x32, 0x00, 0x00,
	0x42, 0x10, 0x00, 0x00, 0xaa, 0x3b, 0x32, 0xe8,
	0x71, 0x55, 0x74, 0x7a, 0xad, 0xd1, 0x4f, 0x30,
	0xe1, 0xa5, 0x41, 0x8d, 0xc3, 0x35, 0xd6, 0x3d,
	0x3c, 0x96, 0x9a, 0x52, 0x29, 0x37, 0x96, 0x62,
	0xab, 0x41, 0x65, 0x65, 0x53, 0x22, 0xac, 0xa2,
	0x72, 0xcc, 0x05, 0x9b, 0xf6, 0x5a, 0x8a, 0xb3,
	0x25, 0x0c, 0xf2, 0x21, 0xe5, 0x51, 0xdd, 0xbb,
	0x3d, 0xd8, 0x43, 0xa2, 0x47, 0x76, 0xe7, 0x73,
	0x50, 0xf3, 0x9c, 0xee, 0x95, 0x9d, 0xb3, 0x9e,
	0x1b, 0x4b
};

static const uint8_t sbc_data6[] = {
	0x80, 0x60, 0x00, 0x3b, 0x00, 0x00, 0xcb, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0xf6, 0xf0, 0x63, 0x33, 0x00, 0x00, 0x32, 0x11,
	0x00, 0x00, 0xa0, 0x17, 0x76, 0xa5, 0xb4, 0x19,
	0x0c, 0xc2, 0xa5, 0x71, 0x25, 0x5c, 0x84, 0xa7,
	0x4c, 0x43, 0xee, 0x92, 0x88, 0x87, 0x61, 0x52,
	0x4f, 0x10, 0xea, 0x3b, 0x51, 0x76, 0xde, 0x24,
	0x29, 0x62, 0x6c, 0xea, 0xe1, 0x57, 0x1a, 0xbd,
	0x71, 0x11, 0x01, 0x51, 0x8c, 0xa6, 0x4d, 0xe6,
	0x36, 0x94, 0xe9, 0x2c, 0x37, 0x0e, 0x9a, 0x0a,
	0x87, 0xbd, 0x9b, 0xa8, 0x6e, 0xef, 0xd3, 0x6d,
	0x9d, 0xdd, 0x26, 0x6c, 0xee, 0xbb, 0xa2, 0xad,
	0x9c, 0xbd, 0x23, 0xa9, 0xf0, 0x64, 0x22, 0x00,
	0x00, 0x32, 0x10, 0x10, 0x00, 0xac, 0x73, 0xc6,
	0x9d, 0xb3, 0xd3, 0x87, 0x1a, 0xb6, 0xd5, 0xf2,
	0xa1, 0x86, 0xe8, 0xa9, 0x9c, 0x30, 0xda, 0xda,
	0x25, 0xad, 0xa3, 0x44, 0xa5, 0x55, 0x43, 0x5b,
	0x62, 0xd1, 0x87, 0x6c, 0x23, 0xe5, 0x9e, 0xc9,
	0x7e, 0x92, 0xa4, 0x1a, 0x4d, 0x78, 0x0a, 0xa0,
	0xd6, 0x9e, 0xec, 0x99, 0x46, 0xca, 0xb5, 0xd4,
	0xac, 0xe9, 0xaa, 0xaa, 0xc5, 0x9b, 0x1e, 0x75,
	0x58, 0x43, 0x5f, 0xac, 0xad, 0xf6, 0x8d, 0x5b,
	0x59, 0xa0, 0xcd, 0x9c, 0xbd, 0x23, 0xb1, 0xf0,
	0x64, 0x22, 0x00, 0x00, 0x32, 0x00, 0x10, 0x00,
	0x86, 0xad, 0x5c, 0xca, 0x72, 0xf8, 0x2b, 0xa0,
	0xd2, 0x2c, 0xe6, 0x85, 0x1e, 0x3a, 0xdc, 0xac,
	0xb3, 0x39, 0x5d, 0x99, 0xd8, 0x57, 0x01, 0x4c,
	0x53, 0x50, 0xdc, 0x34, 0x97, 0x7a, 0x1a, 0x99,
	0x92, 0xf1, 0x34, 0x3c, 0xbd, 0xa2, 0x09, 0x6a,
	0x25, 0x8d, 0x3d, 0x30, 0xc4, 0x45, 0x99, 0x72,
	0xa4, 0x90, 0xe5, 0x06, 0x48, 0x3b, 0x91, 0x18,
	0x74, 0xe8, 0x72, 0xa4, 0x8e, 0xea, 0xd3, 0x76,
	0xe1, 0xdc, 0x19, 0x6d, 0x16, 0x3a, 0x9c, 0xbd,
	0x23, 0x91, 0xf0, 0x74, 0x32, 0x00, 0x00, 0x32,
	0x00, 0x10, 0x00, 0x89, 0xc2, 0x19, 0x48, 0x70,
	0xa8, 0x43, 0x9c, 0xd1, 0xcf, 0x28, 0x74, 0x59,
	0xbd, 0x99, 0x0c, 0x7c, 0x36, 0xdc, 0x21, 0x9d,
	0x76, 0xd7, 0x54, 0x45, 0xf2, 0xdc, 0xef, 0x8b,
	0xad, 0xe2, 0xba, 0x2c, 0x72, 0xb3, 0x53, 0x39,
	0xd6, 0x67, 0x67, 0xa9, 0xbb, 0xbc, 0xcd, 0x1d,
	0x28, 0x36, 0x11, 0x27, 0xb1, 0x91, 0x3a, 0x24,
	0x4d, 0xa5, 0x2e, 0x36, 0xbf, 0x94, 0x2b, 0x6c,
	0x59, 0x69, 0x9a, 0x6e, 0x4b, 0x32, 0x52, 0xfb,
	0x43, 0x9c, 0xbd, 0x23, 0x87, 0xf0, 0x74, 0x32,
	0x00, 0x00, 0x31, 0x10, 0x00, 0x00, 0x75, 0x29,
	0xe3, 0x63, 0x6b, 0x5a, 0xbc, 0x3b, 0x4d, 0xb4,
	0xd6, 0x76, 0x0e, 0x27, 0xd8, 0xcd, 0x02, 0xb4,
	0xa4, 0xd9, 0x9a, 0xa6, 0x93, 0x7b, 0x44, 0x55,
	0x0f, 0xf1, 0x78, 0xd2, 0x9a, 0xa6, 0x6d, 0x1b,
	0x93, 0x5b, 0x9d, 0x97, 0x1c, 0x6a, 0xaa, 0xba,
	0xfe, 0x6d, 0x42, 0xb7, 0x7f, 0x2e, 0x22, 0xde,
	0xcb, 0x2a, 0x45, 0xaa, 0xcd, 0x9e, 0xe8, 0xb8,
	0x28, 0xb4, 0xcd, 0x58, 0x6e, 0x53, 0x65, 0x24,
	0x3e, 0x89, 0x6d, 0x55, 0x9c, 0xbd, 0x23, 0xd1,
	0xf0, 0x63, 0x32, 0x00, 0x00, 0x41, 0x10, 0x10,
	0x00, 0x78, 0x55, 0x7b, 0xa6, 0x16, 0x7c, 0xd3,
	0xb6, 0x36, 0x7c, 0x1e, 0x69, 0x36, 0xda, 0x5c,
	0x3c, 0x16, 0xdc, 0xf7, 0x86, 0x7c, 0xd3, 0x5f,
	0x10, 0xec, 0x93, 0x73, 0x52, 0x1d, 0x6d, 0x4d,
	0x63, 0x43, 0xbe, 0x89, 0xa2, 0x38, 0x98, 0x31,
	0xb8, 0xf7, 0x28, 0xe1, 0x26, 0x69, 0xa1, 0x5e,
	0xb6, 0xc7, 0xc3, 0xc3, 0xe6, 0x95, 0xd9, 0x76,
	0x5a, 0x90, 0xb7, 0x72, 0xaa, 0x8a, 0x1a, 0x3b,
	0x65, 0xe8, 0x22, 0x23, 0xe8, 0xba, 0x44, 0x9c,
	0xbd, 0x23, 0xb0, 0xf0, 0x63, 0x22, 0x00, 0x00,
	0x31, 0x01, 0x00, 0x00, 0x7a, 0xaf, 0x78, 0xf5,
	0x33, 0x80, 0xe5, 0x32, 0xaa, 0xbf, 0x4d, 0x98,
	0xd1, 0xf7, 0x13, 0x36, 0xcd, 0x2b, 0xa5, 0xe5,
	0xd6, 0xb7, 0xd6, 0x5a, 0xb3, 0x36, 0xf1, 0xeb,
	0x47, 0x3f, 0x1e, 0x53, 0x2e, 0xe1, 0xd3, 0xcc,
	0x41, 0x9c, 0xfa, 0x75, 0x2a, 0xc4, 0x3b, 0x4e,
	0xe7, 0x45, 0x79, 0xad, 0xd5, 0x08, 0x8b, 0x6e,
	0x47, 0x26, 0xd5, 0x71, 0x26, 0xbc, 0x2a, 0xbc,
	0x28, 0xce, 0x0e, 0x76, 0x4d, 0x1b, 0x04, 0x6d,
	0x0a, 0xb3
};

static const uint8_t sbc_data7[] = {
	0x80, 0x60, 0x00, 0x3c, 0x00, 0x00, 0xce, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0x1e, 0xf0, 0x64, 0x32, 0x00, 0x00, 0x31, 0x10,
	0x00, 0x00, 0x5f, 0x2c, 0x21, 0x6c, 0xcb, 0x1a,
	0x23, 0xa5, 0x52, 0x67, 0x64, 0xc6, 0xa2, 0x48,
	0xde, 0xaf, 0x3c, 0xcd, 0x7d, 0x96, 0x27, 0x39,
	0x96, 0x32, 0xc3, 0xb9, 0x33, 0x55, 0x67, 0x5a,
	0xdf, 0x39, 0xb0, 0xe5, 0x93, 0xb0, 0x39, 0xdc,
	0x1b, 0x96, 0xf6, 0x3b, 0x98, 0xae, 0x6e, 0xc7,
	0x77, 0xd1, 0x48, 0xd8, 0xd3, 0x22, 0x3b, 0xd5,
	0x21, 0x21, 0x27, 0x3d, 0x54, 0x34, 0x98, 0xe4,
	0x92, 0x8b, 0x86, 0xd2, 0xa9, 0xd1, 0x0e, 0x12,
	0x9c, 0xbd, 0x23, 0x91, 0xf0, 0x64, 0x32, 0x00,
	0x00, 0x31, 0x11, 0x00, 0x00, 0x46, 0xa4, 0x4b,
	0x25, 0x2b, 0x34, 0xb6, 0x5c, 0x94, 0xcd, 0x3c,
	0x86, 0x50, 0xac, 0xaa, 0xb8, 0xec, 0x87, 0x93,
	0xd4, 0x3f, 0x92, 0xe6, 0xfa, 0x9b, 0x5a, 0x68,
	0x21, 0x11, 0xaa, 0x4b, 0x6b, 0xe5, 0xfb, 0x69,
	0xa7, 0x6c, 0x46, 0xb1, 0xb3, 0x0f, 0x68, 0x89,
	0xb6, 0x7e, 0xae, 0xed, 0x14, 0xdc, 0x3d, 0xde,
	0x22, 0xd9, 0xb9, 0x44, 0x50, 0xdb, 0x66, 0xe8,
	0x5b, 0xe3, 0x71, 0x98, 0xcb, 0xab, 0x6d, 0x1d,
	0x1e, 0xf2, 0x4d, 0x9c, 0xbd, 0x23, 0x36, 0xf0,
	0x64, 0x22, 0x00, 0x00, 0x21, 0x10, 0x10, 0x00,
	0x97, 0x52, 0xb6, 0x2d, 0xad, 0x90, 0x88, 0x9e,
	0x36, 0x45, 0x93, 0x30, 0x46, 0xc2, 0x44, 0x19,
	0x75, 0x56, 0xeb, 0x06, 0xb4, 0x2b, 0x53, 0x84,
	0x86, 0xc4, 0x66, 0xdc, 0xac, 0xb1, 0x96, 0x6d,
	0x11, 0xd8, 0x92, 0xd0, 0x3a, 0x33, 0xf2, 0x37,
	0x17, 0x46, 0x55, 0x47, 0x21, 0x8d, 0x06, 0x26,
	0xc7, 0x39, 0x9c, 0xc8, 0xda, 0x76, 0xb4, 0x17,
	0x5b, 0x24, 0xd8, 0x94, 0x23, 0x5f, 0x9e, 0x8f,
	0x3a, 0x8c, 0xf4, 0xdd, 0x3d, 0x4d, 0x9c, 0xbd,
	0x23, 0x10, 0xf0, 0x64, 0x32, 0x00, 0x00, 0x32,
	0x10, 0x10, 0x00, 0x7c, 0x4c, 0x3b, 0x1d, 0x2b,
	0xb3, 0x89, 0x44, 0x34, 0xb1, 0x70, 0xe3, 0x76,
	0x96, 0x45, 0xa2, 0xce, 0xe4, 0xd7, 0xc4, 0x36,
	0x12, 0x76, 0x5c, 0xa9, 0x58, 0x58, 0x51, 0x31,
	0x8c, 0x54, 0x21, 0xe0, 0xb5, 0x4a, 0x81, 0x7c,
	0x28, 0x5d, 0xd8, 0x52, 0x95, 0x3b, 0x2a, 0x3d,
	0x72, 0xdd, 0x74, 0xd5, 0x3e, 0x1a, 0x90, 0x99,
	0x47, 0x3b, 0x59, 0xda, 0xfd, 0x48, 0x6a, 0xab,
	0x5e, 0x20, 0xed, 0x46, 0x6a, 0x34, 0x21, 0xc6,
	0xed, 0x9c, 0xbd, 0x23, 0x05, 0xf0, 0x64, 0x22,
	0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x51, 0x35,
	0x49, 0xaa, 0x2c, 0xe7, 0x62, 0x32, 0xc1, 0x19,
	0x1a, 0xde, 0x26, 0xa2, 0x19, 0x56, 0xd2, 0xf0,
	0xe4, 0x8b, 0x13, 0x20, 0x4d, 0x72, 0x9c, 0xfb,
	0x8c, 0x14, 0x2b, 0x93, 0x51, 0x0b, 0x10, 0xe1,
	0xa5, 0x59, 0x66, 0xc8, 0x08, 0xe8, 0xb7, 0x21,
	0xcd, 0xb5, 0xb3, 0x3b, 0x2c, 0x88, 0x33, 0x12,
	0x65, 0x95, 0x03, 0x7d, 0xc8, 0xf0, 0x5c, 0xb9,
	0x69, 0xa2, 0xa3, 0x8a, 0x6d, 0x33, 0xd2, 0x91,
	0xfd, 0xc4, 0x77, 0x32, 0x9c, 0xbd, 0x23, 0x98,
	0xd0, 0x74, 0x02, 0x00, 0x00, 0x42, 0x10, 0x10,
	0x00, 0x54, 0xb6, 0x21, 0xc2, 0x88, 0xe8, 0x1d,
	0x57, 0x2d, 0x59, 0x27, 0xb2, 0xc5, 0xa5, 0xd3,
	0x57, 0x22, 0xb4, 0x75, 0x1f, 0x1c, 0x54, 0x6d,
	0x54, 0x54, 0x7b, 0x0c, 0x30, 0x77, 0xb0, 0x22,
	0x65, 0x90, 0xfe, 0x4c, 0x42, 0x92, 0xa3, 0xe6,
	0xa9, 0xc4, 0x9b, 0xfc, 0xcd, 0x98, 0x92, 0x7d,
	0x26, 0x34, 0x62, 0x92, 0x9c, 0xd9, 0x34, 0x96,
	0x33, 0x55, 0x01, 0x5b, 0xbd, 0x79, 0x25, 0x51,
	0x78, 0xd1, 0x6c, 0x91, 0xcd, 0x35, 0xa4, 0x9c,
	0xbd, 0x23, 0xb1, 0xf0, 0x64, 0x22, 0x00, 0x00,
	0x32, 0x00, 0x10, 0x00, 0x9e, 0x52, 0x5e, 0x35,
	0x78, 0x47, 0xc3, 0xc4, 0xf2, 0x8c, 0xe8, 0x7b,
	0x62, 0x57, 0x99, 0x4b, 0x7c, 0xbc, 0x9b, 0xde,
	0xef, 0x77, 0x32, 0x8a, 0xde, 0xed, 0x21, 0x6e,
	0x56, 0xc9, 0xda, 0x2e, 0x29, 0x10, 0x3c, 0x66,
	0xbe, 0x65, 0x75, 0x6d, 0xab, 0xf2, 0xaa, 0x8d,
	0x75, 0x66, 0x12, 0xcd, 0xc6, 0x6b, 0x26, 0x72,
	0x99, 0x02, 0xed, 0xa2, 0x85, 0x60, 0x8c, 0xc5,
	0xb0, 0xf4, 0xab, 0x78, 0xa1, 0xcd, 0x6a, 0x8c,
	0xef, 0x3a
};

static const uint8_t sbc_data8[] = {
	0x80, 0x60, 0x00, 0x3d, 0x00, 0x00, 0xd2, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0x43, 0xf0, 0x64, 0x33, 0x00, 0x00, 0x31, 0x10,
	0x00, 0x00, 0xde, 0xb1, 0x9d, 0x42, 0x76, 0xc6,
	0xbb, 0x0c, 0xaf, 0x09, 0x06, 0x71, 0xd1, 0xd8,
	0x1e, 0xce, 0xd5, 0x38, 0xbc, 0x59, 0xaa, 0xa9,
	0x63, 0x93, 0x45, 0x36, 0xea, 0xa8, 0x78, 0xa9,
	0x1c, 0xce, 0x8e, 0xd6, 0x23, 0x7d, 0x26, 0x1e,
	0x62, 0x6b, 0x78, 0xb3, 0x31, 0x91, 0xe8, 0xa3,
	0x84, 0xba, 0xb5, 0x0a, 0x4e, 0x87, 0x57, 0x22,
	0xd8, 0xcc, 0xd9, 0x2d, 0x8a, 0x39, 0x47, 0x20,
	0xaf, 0x46, 0x46, 0x6e, 0x1e, 0x8d, 0x0b, 0x5c,
	0x9c, 0xbd, 0x23, 0x10, 0xf0, 0x64, 0x32, 0x00,
	0x00, 0x32, 0x10, 0x10, 0x00, 0x96, 0x43, 0xc6,
	0x5d, 0xb4, 0x13, 0x86, 0xdc, 0x36, 0xc1, 0xd0,
	0xdb, 0x64, 0xce, 0xc5, 0xda, 0x8c, 0xdc, 0x66,
	0xca, 0x4e, 0x1b, 0x54, 0xe8, 0x4b, 0xca, 0x6b,
	0x9b, 0x0d, 0xa7, 0x4e, 0x94, 0x62, 0x38, 0xcd,
	0xac, 0x83, 0xaa, 0x19, 0xb5, 0x90, 0x75, 0x7b,
	0xb5, 0xb6, 0x70, 0xdc, 0x56, 0xbe, 0x4a, 0x91,
	0xa8, 0x58, 0x4c, 0x6b, 0x40, 0xc8, 0xb7, 0x4a,
	0x2a, 0xa1, 0x5c, 0x25, 0x4d, 0x17, 0x43, 0x59,
	0xea, 0x2d, 0x08, 0x9c, 0xbd, 0x23, 0xbe, 0xf0,
	0x54, 0x32, 0x00, 0x00, 0x32, 0x00, 0x10, 0x00,
	0x6f, 0x94, 0xc3, 0x89, 0x79, 0x6a, 0x78, 0x6b,
	0x09, 0xc2, 0x6d, 0x0c, 0x98, 0xe5, 0xb1, 0x9e,
	0x4c, 0x37, 0x56, 0xbb, 0xd9, 0x89, 0x49, 0x46,
	0x7b, 0x0a, 0xb0, 0x5e, 0xcf, 0xa9, 0xd4, 0x4c,
	0x59, 0xfd, 0xb3, 0xdb, 0x9b, 0x5f, 0x47, 0x7a,
	0xe8, 0x77, 0xd9, 0x12, 0xbe, 0xae, 0xb9, 0xde,
	0x32, 0x1d, 0xde, 0xf4, 0x31, 0x97, 0x33, 0x45,
	0xa4, 0xec, 0x64, 0x78, 0x54, 0x9c, 0x0e, 0x42,
	0xf7, 0x0c, 0x8b, 0x95, 0x18, 0xbb, 0x9c, 0xbd,
	0x23, 0x10, 0xf0, 0x64, 0x32, 0x00, 0x00, 0x32,
	0x10, 0x10, 0x00, 0xb9, 0xaa, 0x55, 0xd6, 0x95,
	0xaf, 0x49, 0x12, 0xc3, 0xc6, 0xad, 0x26, 0x68,
	0x6f, 0x25, 0xdc, 0x6e, 0xdd, 0xeb, 0x43, 0x91,
	0xd9, 0xbc, 0xe9, 0x6d, 0x22, 0x2c, 0xde, 0xec,
	0xf9, 0x4e, 0x2b, 0x25, 0x9d, 0x09, 0x88, 0x6c,
	0x38, 0x25, 0xad, 0x8d, 0x89, 0x74, 0x36, 0x3a,
	0x50, 0xed, 0x76, 0xb9, 0x3a, 0x15, 0x4e, 0xd7,
	0x68, 0xba, 0xb6, 0x12, 0x69, 0x38, 0x56, 0x1a,
	0x52, 0x67, 0x6a, 0x66, 0x23, 0x06, 0x31, 0xac,
	0x64, 0x9c, 0xbd, 0x23, 0x05, 0xf8, 0x64, 0x32,
	0x10, 0x00, 0x31, 0x10, 0x00, 0x00, 0x50, 0x86,
	0xba, 0x69, 0x2e, 0x8e, 0xbe, 0x4a, 0xc6, 0x0d,
	0x17, 0x41, 0xd4, 0xc3, 0xc6, 0x69, 0x2a, 0x2b,
	0x85, 0x3b, 0xaa, 0x46, 0xf7, 0x16, 0x75, 0xc4,
	0xe5, 0xd8, 0xcc, 0xd7, 0x63, 0x7b, 0x99, 0xd8,
	0x54, 0x7e, 0xa3, 0x32, 0xea, 0x90, 0x8e, 0x66,
	0x60, 0x71, 0x76, 0x2d, 0x0e, 0x09, 0xbd, 0x45,
	0xa9, 0xbd, 0xb7, 0x74, 0xb3, 0xb7, 0xc6, 0xbb,
	0x67, 0x66, 0xe4, 0xcd, 0x8a, 0xee, 0xf3, 0x98,
	0x83, 0xd9, 0x99, 0xb4, 0x9c, 0xbd, 0x23, 0xa4,
	0xf0, 0x64, 0x32, 0x00, 0x00, 0x32, 0x10, 0x00,
	0x00, 0x38, 0xa4, 0x91, 0x68, 0xa6, 0xa5, 0x0a,
	0x9b, 0x94, 0xa1, 0x03, 0xa5, 0x5a, 0xa7, 0xaa,
	0x76, 0xab, 0x53, 0x3b, 0x12, 0xd4, 0x68, 0xf5,
	0xc3, 0x38, 0x6d, 0x1c, 0x68, 0x77, 0x11, 0xdb,
	0x66, 0x90, 0xe7, 0x43, 0x7a, 0xb2, 0x1e, 0x06,
	0x68, 0xc6, 0xbb, 0xaf, 0x2d, 0x8e, 0xd8, 0x86,
	0x5d, 0x9e, 0x5c, 0xf1, 0x74, 0x32, 0x9b, 0x1e,
	0x74, 0x96, 0xa0, 0x9c, 0x45, 0x0c, 0xc8, 0x2d,
	0x85, 0xcd, 0x98, 0xa2, 0x4e, 0xbf, 0x42, 0x9c,
	0xbd, 0x23, 0xbd, 0xf0, 0x73, 0x32, 0x00, 0x00,
	0x32, 0x10, 0x00, 0x00, 0x1d, 0xcc, 0x37, 0xb3,
	0x43, 0xe9, 0x75, 0xca, 0xe8, 0xc7, 0x95, 0x99,
	0x88, 0x98, 0x4f, 0x21, 0xfb, 0x14, 0x61, 0x4e,
	0x33, 0x92, 0xb9, 0x19, 0x89, 0xf0, 0x57, 0xaf,
	0x2f, 0x6e, 0x14, 0x49, 0x5e, 0x1c, 0x4a, 0x77,
	0x44, 0x3c, 0x95, 0x72, 0x07, 0x76, 0xbf, 0x4e,
	0x0e, 0xcf, 0x2c, 0x95, 0xc1, 0xfa, 0x1c, 0x53,
	0xa8, 0xb2, 0xb3, 0xc7, 0x66, 0xe5, 0x97, 0xad,
	0x4c, 0x9f, 0xd0, 0xed, 0x9d, 0x5c, 0x58, 0x55,
	0x2e, 0xcb
};

static const uint8_t sbc_data9[] = {
	0x80, 0x60, 0x00, 0x3e, 0x00, 0x00, 0xd5, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0xf2, 0xf8, 0x64, 0x32, 0x00, 0x00, 0x41, 0x00,
	0x00, 0x00, 0x87, 0x52, 0xcf, 0x6c, 0x6f, 0x93,
	0x99, 0xae, 0x75, 0x49, 0x13, 0x2d, 0x22, 0xbd,
	0xd7, 0x59, 0x9b, 0xe8, 0x35, 0x0b, 0x2d, 0xaf,
	0x06, 0xb8, 0xf5, 0x45, 0xec, 0x98, 0xa9, 0xb3,
	0x1c, 0xe5, 0x95, 0x65, 0xa9, 0xc0, 0x60, 0xcc,
	0x70, 0x59, 0x73, 0x9d, 0x91, 0x32, 0xa9, 0x5b,
	0x33, 0xda, 0x5f, 0x98, 0x77, 0x8c, 0x88, 0x8a,
	0xee, 0xeb, 0xd1, 0x59, 0x79, 0x9d, 0x9a, 0x68,
	0x0f, 0xb3, 0xb7, 0x24, 0x99, 0xc6, 0x66, 0xbd,
	0x9c, 0xbd, 0x23, 0x11, 0xf8, 0x64, 0x32, 0x00,
	0x00, 0x31, 0x10, 0x00, 0x00, 0xc5, 0x55, 0xd5,
	0x10, 0x75, 0x25, 0x4b, 0xb9, 0xaa, 0xa3, 0x48,
	0x8a, 0xa9, 0x39, 0x1b, 0x0f, 0xad, 0xa6, 0xa4,
	0x1e, 0xe7, 0xb5, 0x05, 0x69, 0x43, 0x73, 0x1f,
	0x87, 0x88, 0x30, 0xc4, 0x29, 0x2e, 0x6c, 0x19,
	0x8d, 0xb9, 0x45, 0x32, 0x4e, 0x56, 0x33, 0x20,
	0x8e, 0xe3, 0x28, 0x42, 0x29, 0xcb, 0x9b, 0x0c,
	0xbc, 0xba, 0xdc, 0x21, 0xe5, 0x47, 0x35, 0x6b,
	0xbb, 0xf3, 0x20, 0x53, 0x76, 0x50, 0xdd, 0x15,
	0x70, 0xcc, 0xda, 0x9c, 0xbd, 0x23, 0x43, 0xf0,
	0x64, 0x33, 0x00, 0x00, 0x31, 0x10, 0x00, 0x00,
	0x93, 0xc2, 0x1e, 0x5a, 0x55, 0xc6, 0x43, 0x61,
	0x8a, 0xfe, 0xc7, 0x88, 0xf1, 0xd2, 0x26, 0xb1,
	0x51, 0x49, 0xb2, 0x9d, 0xa7, 0x58, 0xab, 0xa4,
	0x4c, 0x75, 0x1a, 0x2e, 0xa8, 0x81, 0x9b, 0xdb,
	0x18, 0xc7, 0x93, 0x85, 0x52, 0x08, 0x7c, 0xb4,
	0x59, 0x42, 0x89, 0xb2, 0x7c, 0xb9, 0x72, 0x9a,
	0x50, 0xe9, 0x0e, 0x30, 0xbc, 0x62, 0x1d, 0xc7,
	0xe7, 0x35, 0x9b, 0x3c, 0x14, 0xf1, 0x4e, 0x57,
	0xac, 0x9c, 0x5d, 0x2c, 0xea, 0x22, 0x9c, 0xbd,
	0x23, 0x47, 0xf0, 0x64, 0x32, 0x00, 0x00, 0x42,
	0x00, 0x00, 0x00, 0x7c, 0x4a, 0xcc, 0xac, 0xaf,
	0xd7, 0x95, 0xdd, 0x1d, 0x4e, 0xea, 0xad, 0x0b,
	0xba, 0xab, 0x78, 0xa1, 0x47, 0x2a, 0x0f, 0x37,
	0xb2, 0xf1, 0xa2, 0x23, 0x21, 0x57, 0x89, 0xbc,
	0x64, 0x3a, 0x7a, 0x16, 0x9b, 0x97, 0x71, 0xa0,
	0x89, 0x30, 0xb0, 0x82, 0xc9, 0x17, 0xa6, 0x62,
	0xdb, 0x1a, 0xc3, 0x64, 0xa6, 0x5a, 0x6a, 0x3b,
	0xeb, 0x8b, 0x0e, 0xed, 0xea, 0xad, 0xe1, 0x61,
	0xf4, 0x49, 0xc4, 0x36, 0x36, 0xf6, 0x39, 0x79,
	0x4b, 0x9c, 0xbd, 0x23, 0xe1, 0xf0, 0x64, 0x33,
	0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0xce, 0x3f,
	0x2d, 0x29, 0x94, 0x18, 0xa1, 0xd9, 0xbf, 0x04,
	0xd4, 0xb4, 0x09, 0xd0, 0x9d, 0x97, 0xa4, 0xfa,
	0x13, 0x71, 0x13, 0x63, 0x7f, 0x81, 0xe6, 0xcb,
	0xe4, 0x91, 0x3b, 0xfb, 0x5c, 0x6d, 0xc7, 0x6d,
	0xf4, 0x42, 0xd1, 0x0f, 0xd1, 0x8a, 0x07, 0xa1,
	0x70, 0x39, 0xb9, 0x95, 0xca, 0x46, 0xb6, 0x1d,
	0xc3, 0x06, 0xc7, 0xab, 0xd3, 0x21, 0xe5, 0x05,
	0x52, 0xa5, 0x5e, 0x1a, 0x71, 0x5b, 0x9c, 0x4d,
	0x95, 0x48, 0x81, 0xef, 0x9c, 0xbd, 0x23, 0xbc,
	0xf0, 0x64, 0x32, 0x00, 0x00, 0x32, 0x00, 0x00,
	0x00, 0x8c, 0x54, 0xee, 0x9d, 0x76, 0xe7, 0xdd,
	0x9b, 0xae, 0x12, 0xb3, 0x4a, 0xe9, 0x3f, 0xe9,
	0x76, 0x60, 0xc8, 0xbb, 0xcc, 0xd2, 0xd8, 0xd9,
	0x79, 0x63, 0x1b, 0xa2, 0xd3, 0x34, 0xf9, 0x6b,
	0xfc, 0xc6, 0x8f, 0x72, 0x72, 0x5c, 0xc9, 0x71,
	0x67, 0xb3, 0x61, 0xb4, 0xbc, 0x17, 0x3a, 0xae,
	0x69, 0x08, 0x8c, 0x67, 0xab, 0x21, 0x64, 0x24,
	0xaa, 0x22, 0x5e, 0x8c, 0xe0, 0x74, 0x19, 0x18,
	0x2e, 0x1f, 0x32, 0xb9, 0x85, 0x85, 0x2d, 0x9c,
	0xbd, 0x23, 0x90, 0xf8, 0x74, 0x32, 0x00, 0x00,
	0x31, 0x00, 0x00, 0x00, 0x80, 0x68, 0xb8, 0x2f,
	0x90, 0x53, 0xdf, 0x0b, 0x92, 0x11, 0x43, 0xde,
	0x8e, 0x48, 0xde, 0x8b, 0xc6, 0x67, 0x04, 0x10,
	0x74, 0xf0, 0xf6, 0x6e, 0x4c, 0x73, 0x1b, 0x70,
	0x49, 0xd1, 0x32, 0xc5, 0xa9, 0xcd, 0x68, 0x62,
	0xd5, 0x29, 0xec, 0xc9, 0x76, 0xe6, 0x9f, 0x95,
	0x33, 0x1b, 0xd3, 0x54, 0x15, 0x60, 0x82, 0xe6,
	0x32, 0xab, 0xf0, 0x5c, 0xca, 0x6b, 0x9a, 0xf0,
	0xf9, 0x8d, 0xe8, 0x6e, 0x89, 0xca, 0x4d, 0x6a,
	0x62, 0x29
};

static const uint8_t sbc_data10[] = {
	0x80, 0x60, 0x00, 0x3f, 0x00, 0x00, 0xd9, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0xbd, 0x23,
	0x71, 0xf8, 0x63, 0x22, 0x00, 0x00, 0x42, 0x00,
	0x00, 0x00, 0x42, 0x1a, 0xe6, 0x27, 0x24, 0xc6,
	0x6c, 0x40, 0x31, 0x9e, 0xca, 0x77, 0x08, 0x20,
	0xe9, 0x50, 0xf4, 0xf7, 0x5c, 0x99, 0xa7, 0x1a,
	0x8c, 0x42, 0xc5, 0xac, 0x0f, 0xbd, 0x67, 0xaa,
	0x6b, 0x2d, 0x24, 0xf2, 0x8e, 0x49, 0x48, 0xea,
	0x89, 0xee, 0x39, 0x1c, 0xde, 0x4a, 0x1a, 0x01,
	0x5b, 0xa8, 0xbd, 0x38, 0xa7, 0x46, 0xf7, 0x58,
	0xd9, 0xed, 0xa4, 0x98, 0xb3, 0x26, 0x43, 0xdc,
	0x49, 0x46, 0xaf, 0x94, 0x0f, 0x25, 0x2e, 0x0b,
	0x9c, 0xbd, 0x23, 0xd4, 0xf0, 0x64, 0x33, 0x00,
	0x00, 0x31, 0x01, 0x00, 0x00, 0x89, 0x54, 0xf0,
	0x99, 0x37, 0x2c, 0x34, 0x16, 0x0a, 0x5e, 0xb5,
	0x84, 0xa9, 0xd6, 0x6c, 0xae, 0x56, 0xbc, 0x51,
	0xdd, 0xc8, 0xa7, 0x62, 0xa3, 0x3a, 0xa6, 0xf4,
	0x2e, 0x77, 0x6e, 0x9c, 0x69, 0xb0, 0xe9, 0x9a,
	0xac, 0xbd, 0xdc, 0xf3, 0x74, 0x47, 0x43, 0xaf,
	0x6e, 0x3e, 0xd8, 0x8b, 0x69, 0xe0, 0x27, 0x4d,
	0xb1, 0x3a, 0x02, 0xa9, 0x6e, 0xb9, 0x54, 0x9c,
	0xbc, 0xe4, 0xdb, 0xcc, 0x77, 0x84, 0xda, 0x91,
	0xec, 0xcf, 0x93, 0x9c, 0xbd, 0x23, 0x19, 0xf8,
	0x64, 0x31, 0x10, 0x00, 0x41, 0x10, 0x00, 0x00,
	0x69, 0x6c, 0xd5, 0xd4, 0x4e, 0x26, 0xcc, 0xa9,
	0xc6, 0x59, 0x51, 0x54, 0xd5, 0xe0, 0x2e, 0x5a,
	0xc3, 0x3a, 0x27, 0x53, 0x95, 0x25, 0xad, 0x28,
	0x69, 0xf6, 0xea, 0x9b, 0x2d, 0x54, 0xde, 0x7c,
	0x61, 0x4f, 0x5b, 0xb0, 0x73, 0xb6, 0x43, 0x91,
	0x75, 0x86, 0xc1, 0x4e, 0x81, 0xce, 0xd6, 0x85,
	0xb2, 0x41, 0xe2, 0xee, 0x36, 0x57, 0xba, 0x62,
	0x46, 0xe4, 0xb6, 0x0c, 0xa7, 0x19, 0xe8, 0x8d,
	0xa8, 0xeb, 0x92, 0xdb, 0x36, 0xd4, 0x9c, 0xbd,
	0x23, 0x5f, 0xf8, 0x64, 0x22, 0x10, 0x00, 0x41,
	0x00, 0x00, 0x00, 0x4a, 0x4d, 0x55, 0xd4, 0x6d,
	0xf6, 0xb5, 0xd8, 0xae, 0x2f, 0x56, 0xc8, 0xc0,
	0x47, 0x2a, 0x17, 0x10, 0x1d, 0x86, 0x1c, 0xe9,
	0xd1, 0xa2, 0x6b, 0x25, 0x3c, 0xb6, 0xb6, 0x54,
	0x88, 0x9f, 0x41, 0x10, 0xf1, 0x32, 0xbb, 0xd5,
	0x96, 0xa3, 0x79, 0x07, 0x3b, 0xc6, 0x31, 0xd4,
	0xb8, 0x79, 0x01, 0xc2, 0x22, 0xb1, 0x1f, 0xc6,
	0xdb, 0x9e, 0x24, 0x04, 0x9a, 0x84, 0x3c, 0x94,
	0xa0, 0x92, 0x93, 0x92, 0xa3, 0x29, 0x34, 0x8e,
	0x73, 0x9c, 0xbd, 0x23, 0x43, 0xf8, 0x64, 0x23,
	0x10, 0x00, 0x32, 0x00, 0x00, 0x00, 0x72, 0x4b,
	0x21, 0xe5, 0x28, 0xb6, 0x4c, 0x3e, 0xdd, 0x0b,
	0x05, 0x67, 0xdb, 0xbb, 0x98, 0x97, 0x6c, 0xa7,
	0xa1, 0x26, 0xeb, 0x95, 0x21, 0x5c, 0x2c, 0x62,
	0xef, 0xac, 0x65, 0x89, 0xc4, 0x99, 0xce, 0xb8,
	0xa6, 0xd7, 0xc1, 0x9e, 0x23, 0x17, 0x14, 0xc3,
	0xb6, 0x67, 0x65, 0x19, 0x68, 0xbc, 0xeb, 0xde,
	0xf1, 0x22, 0x6b, 0x74, 0xa2, 0x23, 0x4d, 0xbd,
	0x75, 0x3c, 0x85, 0xe9, 0x6d, 0x67, 0x96, 0xc5,
	0x6e, 0x2e, 0xaf, 0x36, 0x9c, 0xbd, 0x23, 0x39,
	0xf8, 0x64, 0x43, 0x10, 0x00, 0x42, 0x20, 0x00,
	0x00, 0xcf, 0x5b, 0xa6, 0xfb, 0x76, 0xd4, 0x76,
	0xcc, 0x4e, 0x85, 0x4c, 0xd9, 0xd1, 0xa8, 0x59,
	0xdb, 0x4d, 0xb5, 0x67, 0x4c, 0xa2, 0xa6, 0xa1,
	0x98, 0xb3, 0xd4, 0xd4, 0xaa, 0xca, 0x46, 0x9c,
	0xc5, 0xe1, 0x2d, 0x53, 0x8b, 0x63, 0xa9, 0xaa,
	0x73, 0xd6, 0x67, 0x1f, 0x2f, 0x0d, 0x0e, 0xd4,
	0xf2, 0x50, 0x49, 0xd2, 0x71, 0xba, 0x73, 0x4c,
	0x63, 0x36, 0xbb, 0xb1, 0x53, 0x53, 0xb6, 0x76,
	0x11, 0x48, 0x25, 0x25, 0x69, 0xae, 0x8a, 0x9c,
	0xbd, 0x23, 0xae, 0xf8, 0x65, 0x32, 0x10, 0x00,
	0x43, 0x20, 0x00, 0x00, 0x48, 0x74, 0x4b, 0x87,
	0x8e, 0x31, 0x85, 0xa3, 0x2d, 0x29, 0x91, 0xb4,
	0xdd, 0x27, 0x46, 0x46, 0x73, 0xa2, 0xd6, 0xb6,
	0xed, 0x64, 0x4b, 0x06, 0xda, 0xf0, 0xd0, 0x58,
	0xf5, 0x32, 0x1a, 0x63, 0x1c, 0x9c, 0xc3, 0x68,
	0x4b, 0x21, 0xe9, 0xb7, 0xed, 0x25, 0x79, 0x15,
	0xc6, 0x25, 0x4a, 0xd8, 0xc7, 0x5a, 0xc5, 0x4b,
	0x16, 0x23, 0x30, 0xab, 0x88, 0xf6, 0xc7, 0x63,
	0x56, 0xe3, 0x1f, 0xa3, 0x15, 0xda, 0x55, 0x22,
	0x57, 0xb3
};

struct bt_audio_data_len sbc_array[] = {
	{sizeof(sbc_data1), sbc_data1},
	{sizeof(sbc_data2), sbc_data2},
	{sizeof(sbc_data3), sbc_data3},
	{sizeof(sbc_data4), sbc_data4},
	{sizeof(sbc_data5), sbc_data5},
	{sizeof(sbc_data6), sbc_data6},
	{sizeof(sbc_data7), sbc_data7},
	{sizeof(sbc_data8), sbc_data8},
	{sizeof(sbc_data9), sbc_data9},
	{sizeof(sbc_data10), sbc_data10}
};

uint8_t *bt_test_get_sbc_data(uint16_t *len)
{
	static uint8_t index;

	index++;
	if (index >= ARRAY_SIZE(sbc_array)) {
		index = 0;
	}

	*len = sbc_array[index].len;
	return (uint8_t *)sbc_array[index].data;
}
