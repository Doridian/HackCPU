#ifndef BOOTLOADER_H_INCLUDED
#define BOOTLOADER_H_INCLUDED

#include <stdint.h>

static uint8_t BOOTLOADER[] = {
    0x06, 0x01, 0x8d, 0xef, 0xbe, 0xad, 0xde, 0x35, 0xef, 0x13, 0xad, 0xde,
    0xef, 0xf3, 0x72, 0x21, 0xfe, 0xbe, 0xbc, 0xcf, 0xfe, 0x9c, 0xbc, 0xed,
    0xed, 0x6e, 0xac, 0xde, 0xc0, 0x6e, 0xab, 0xde, 0xec, 0xbe, 0x89, 0xd3,
    0x54, 0xbe, 0xae, 0xde, 0xf8, 0xb3, 0xa8, 0xde, 0xcd, 0x6e, 0x16, 0xde,
    0xed, 0x6e, 0xac, 0xde, 0xc0, 0x6e, 0xaa, 0xde, 0xef, 0xa3, 0x52, 0x21,
    0xe7, 0xae, 0xaa, 0xc3, 0xee, 0xbe, 0xaf, 0xde, 0xed, 0xae, 0xaf, 0x0e,
    0xee, 0xbe, 0x82, 0x0e, 0xee, 0xbe, 0xad, 0x7f, 0xe7, 0x13, 0x51, 0xdf,
    0xef, 0xf4, 0xa5, 0x93, 0xef, 0xbf, 0xad, 0x8a, 0xe7, 0xe3, 0xad, 0xdf,
    0xee, 0x90, 0xac, 0xdf, 0x02, 0xbf, 0xad, 0xde, 0xef, 0xbe, 0xbc, 0xf3,
    0x15, 0x60, 0xbc, 0xe3, 0x55, 0x00, 0xaa, 0xc3, 0xeb, 0xbe, 0xad, 0x3f,
    0xea, 0xb9, 0xf0, 0xdc, 0xef, 0xbe, 0xa0, 0xde, 0xee, 0xb6, 0xa0, 0xdf,
    0xef, 0x8e, 0xa0, 0xde, 0xef, 0x9a, 0xa0, 0x5f, 0xef, 0x8a, 0x01, 0x82,
    0x11, 0x22, 0x11, 0x33, 0x00, 0x06, 0x07, 0x0d, 0x02, 0x00, 0x00, 0x10,
    0x0b, 0x1d, 0x02, 0x00, 0x07, 0x01, 0x11, 0x11, 0x08, 0x0d, 0x02, 0x00,
    0x00, 0xed, 0x00, 0x00, 0x00, 0x24, 0x0d, 0xa4, 0x00, 0x24, 0x8d, 0xba,
    0x00, 0x24, 0x9d, 0xba, 0x00, 0x2e, 0x38, 0x31
};

#endif // BOOTLOADER_H_INCLUDED
