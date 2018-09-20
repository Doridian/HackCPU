#ifndef BOOTLOADER_H_INCLUDED
#define BOOTLOADER_H_INCLUDED

#include <stdint.h>

static uint8_t BOOTLOADER[] = {
	0x82, 0x40, 0xad, 0x03, 0x53, 0x0b, 0xb0, 0xef, 0xbe, 0xad, 0xde, 0x89,
	0xe0, 0x50, 0xb5, 0x2e, 0x1b, 0xbd, 0x04, 0x5e, 0xf7, 0x50, 0xb5, 0x32,
	0x0c, 0xbd, 0x13, 0x5c, 0xf7, 0x54, 0xad, 0x13, 0x03, 0x53, 0x0b, 0xb0,
	0xef, 0xd3, 0x8d, 0xde, 0x03, 0x53, 0x0b, 0xc6, 0xe0, 0xbe, 0xa2, 0xcf,
	0x0c, 0x71, 0x04, 0x83, 0xe0, 0xfa, 0xa2, 0x8b, 0x03, 0x4b, 0x0d, 0xad,
	0xd5, 0xbe, 0xad, 0xde, 0x00, 0x83, 0x0a, 0xb0, 0xef, 0xbe, 0x86, 0x0e,
	0x05, 0x53, 0x0b, 0xb0, 0xeb, 0xbe, 0xb8, 0xd3, 0x0a, 0x53, 0x0b, 0xb0,
	0xef, 0xe3, 0x3a, 0xde, 0x03, 0x53, 0x0e, 0xe1, 0xf6, 0xee, 0xae, 0x0e,
	0x1f, 0x53, 0x0b, 0xb0, 0xef, 0xe3, 0xd6, 0xde, 0x03, 0x53, 0x0e, 0xe1,
	0xec, 0xee, 0xae, 0x0e, 0x03, 0x53, 0x0b, 0xb0, 0xc4, 0x6e, 0xad, 0xde,
	0x03, 0x53, 0x8e, 0x52, 0x4f, 0x4d, 0x20, 0x74, 0x6f, 0x6f, 0x20, 0x73,
	0x68, 0x6f, 0x72, 0x74, 0x20, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x20,
	0x70, 0x72, 0x65, 0x73, 0x65, 0x6e, 0x74, 0xcb, 0x02, 0x53, 0x56, 0x7c,
	0xef, 0xbe, 0xad, 0xdb, 0x52, 0x49, 0x5b, 0xb3, 0x3f, 0xb2, 0xad, 0xde,
	0x03, 0x53, 0x56, 0x70, 0xef, 0xbe, 0xad, 0xdb, 0x52, 0x50, 0x5b, 0xb3,
	0x3f, 0xbe, 0xad, 0xde, 0x03, 0x78, 0xdb, 0xb0, 0xef, 0xbe, 0xad, 0x5b,
	0x52, 0x4f, 0x4d, 0x20, 0x74, 0x6f, 0x6f, 0x20, 0x6c, 0x6f, 0x6e, 0x67,
	0xef, 0x9f, 0xad, 0x9f, 0x06, 0x1e, 0xc5, 0xb0, 0xef, 0xbe, 0xad, 0x83,
	0xdd, 0x53, 0x0b, 0xb0, 0xea, 0xef, 0xad, 0x33, 0x01, 0x53, 0x0b, 0xb0,
	0xef, 0xbb, 0x80, 0xda, 0x03, 0x53, 0x0b, 0xa5, 0xcb, 0xa0, 0xfd, 0xdd,
	0xd3, 0x52, 0x0b, 0xb0, 0xef, 0x95, 0x7d, 0xd9, 0x03, 0x53, 0x0b, 0xb3,
	0xef, 0xbd, 0x7d, 0xde, 0x03, 0x53, 0x0b, 0xb3, 0x3f, 0xbf, 0xad, 0xde,
	0x03, 0x78, 0xdb, 0xb1, 0xef, 0xbe, 0xad, 0xe8, 0xc3, 0x55, 0xc6, 0xb1,
	0xeb, 0xbe, 0xad, 0xde, 0x6f, 0x53, 0x7d, 0xf0, 0xc1, 0xae, 0x55, 0x9e,
	0xee, 0x43, 0xf3, 0xb0, 0xef, 0xbe, 0xad, 0xde, 0x03, 0x53, 0x0b, 0xbf,
	0xc2, 0x44, 0x73, 0x64, 0xbd, 0x5c, 0x36, 0x7a, 0x11, 0x0e, 0xa6, 0xde,
	0x5e, 0x16, 0x0a, 0xb0, 0xef, 0xbb, 0xfc, 0xde, 0x0e, 0x53, 0x0a, 0xb0,
	0xef, 0xb8, 0xa0, 0xdf, 0x03, 0x53, 0x0b, 0x9c, 0xe2, 0xbe, 0xad, 0xde,
	0x03, 0x73, 0x0e, 0xb0, 0xb2, 0xcc, 0xac, 0xde, 0x03, 0x56, 0x5a, 0x38,
	0x40, 0xa2, 0x0f, 0x22, 0x0f, 0x33, 0x0f, 0x44, 0x20, 0xa5, 0x20, 0xb5,
	0x0f, 0x11, 0x0f, 0x55, 0x18, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d,
	0x90, 0x01, 0x00, 0x00, 0x05, 0x51, 0x0f, 0xea, 0x05, 0x0f, 0xeb, 0x15,
	0x03, 0x0f, 0xea, 0x15, 0x07, 0x0f, 0xeb, 0x15, 0x0b, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x89, 0x0f, 0x11, 0x0f, 0x55, 0x18, 0xd0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xf0, 0xff, 0xff
};

#endif // BOOTLOADER_H_INCLUDED
