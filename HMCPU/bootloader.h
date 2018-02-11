#ifndef BOOTLOADER_H_INCLUDED
#define BOOTLOADER_H_INCLUDED

#include <stdint.h>

static uint8_t BOOTLOADER[] = {
	0x08, 0x01, 0xad, 0x03, 0x53, 0x0b, 0xb0, 0xef, 0xbe, 0xad, 0xde, 0x38,
	0xef, 0x73, 0xad, 0xde, 0x03, 0x53, 0x0b, 0xdd, 0x30, 0x41, 0x92, 0xde,
	0x10, 0x53, 0x18, 0xa1, 0xfc, 0x9c, 0xbe, 0xed, 0x10, 0x17, 0x18, 0xe5,
	0xeb, 0x6e, 0xac, 0xde, 0x03, 0x53, 0x39, 0x60, 0xe9, 0xbe, 0xad, 0xde,
	0x06, 0x53, 0x12, 0xbd, 0xe6, 0xbe, 0xad, 0xde, 0x23, 0x83, 0x78, 0xb0,
	0xef, 0xbe, 0xa9, 0x0e, 0x1f, 0x53, 0x0b, 0xb0, 0xeb, 0x6e, 0xfa, 0xde,
	0x03, 0x53, 0x0f, 0x60, 0xef, 0xbe, 0xad, 0xde, 0x31, 0x83, 0x0b, 0xb0,
	0xef, 0xbe, 0x99, 0x52, 0x4f, 0x4d, 0x20, 0x74, 0x6f, 0x6f, 0x20, 0x73,
	0x68, 0x6f, 0x72, 0x74, 0x20, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x20,
	0x70, 0x72, 0x65, 0x73, 0x65, 0x6e, 0x74, 0xb4, 0x3f, 0xbf, 0xad, 0xde,
	0x03, 0x61, 0xdb, 0xb7, 0xef, 0xbe, 0xad, 0xde, 0x1e, 0xac, 0xf4, 0x8f,
	0xef, 0xb4, 0xbd, 0xd7, 0x1e, 0x52, 0x0b, 0xb0, 0xef, 0xba, 0xad, 0xda,
	0x13, 0x57, 0xdb, 0xb1, 0xef, 0xbe, 0xad, 0xec, 0xd3, 0x52, 0x0b, 0xb0,
	0xef, 0xbe, 0x6c, 0xd4, 0xce, 0xab, 0x08, 0xb0, 0xef, 0xbe, 0xc1, 0xd4,
	0x6e, 0x53, 0x1b, 0xb0, 0xef, 0xbe, 0xdb, 0xd4, 0x7e, 0x53, 0x1b, 0xb0,
	0xef, 0xbf, 0x83, 0xdf, 0x02, 0xbe, 0x0a, 0xb0, 0xef, 0xbe, 0xad, 0xde,
	0x03, 0x53, 0x0b, 0xa3, 0xc2, 0x44, 0x73, 0x64, 0xbd, 0x40, 0x36, 0x7a,
	0x11, 0x0e, 0xa6, 0xd7, 0x1e, 0x5b, 0x0b, 0xb0, 0xef, 0xbe, 0x4c, 0xd9,
	0x0a, 0x2e, 0x0f, 0xb0, 0xef, 0xbe, 0xad, 0xd3, 0x03, 0x52, 0x0b, 0xb0,
	0xe5, 0xb3, 0xac, 0xde, 0x03, 0x53, 0x38, 0xbd, 0xef, 0xbe, 0xad, 0xde,
	0x24, 0x5e, 0xef, 0xb0, 0xef, 0xbe, 0x9a, 0x01, 0xa2, 0x13, 0x22, 0x13,
	0x33, 0x00, 0x08, 0x00, 0x10, 0x0d, 0x1d, 0x04, 0x00, 0x00, 0x00, 0x0a,
	0x01, 0x13, 0x11, 0x0a, 0x0d, 0x04, 0x00, 0x00, 0x00, 0x00, 0xed, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x27, 0x0d, 0x0b, 0x01, 0x00, 0x00, 0x27, 0xad,
	0x2b, 0x01, 0x00, 0x00, 0x27, 0xbd, 0x2b, 0x01, 0x00, 0x00, 0x31, 0x13,
	0xfa, 0x34, 0x01, 0x00, 0x00, 0x38, 0x08, 0x08, 0x31
};

#endif // BOOTLOADER_H_INCLUDED
