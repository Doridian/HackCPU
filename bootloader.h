#ifndef BOOTLOADER_H_INCLUDED
#define BOOTLOADER_H_INCLUDED

#include <stdint.h>

static uint8_t BOOTLOADER[] = {
    0x01, 0x6d, 0xef, 0xbe, 0xad, 0xde, 0x35, 0xdf, 0x12, 0xbe, 0xad, 0xd8,
    0xe9, 0xb8, 0xab, 0xdf, 0x12, 0xba, 0xad, 0xd8, 0xe9, 0xb8, 0xab, 0xdf,
    0x12, 0xb6, 0xad, 0xd8, 0xe9, 0xb8, 0xab, 0xde, 0x72, 0xbe, 0xad, 0xde,
    0xa2, 0x61, 0x52, 0xcf, 0xef, 0xaf, 0xbc, 0xcf, 0xcd, 0xaf, 0x9e, 0xdc,
    0x3f, 0xbf, 0xad, 0xf1, 0x3f, 0xb8, 0xad, 0xdd, 0xef, 0x9a, 0xa0, 0x79,
    0xef, 0xbd, 0xad, 0xc9, 0xe2, 0xbb, 0xad, 0xfc, 0x3f, 0x19, 0xad, 0xdc,
    0x3f, 0xbf, 0xad, 0xf1, 0x3f, 0xb9, 0xad, 0xde, 0xf2, 0x41, 0x52, 0xd6,
    0xff, 0xbc, 0xad, 0xdc, 0xff, 0xbc, 0x7d, 0xdf, 0xef, 0x91, 0x7d, 0xdf,
    0xef, 0xbe, 0x9c, 0xd6, 0xd2, 0xbe, 0xaf, 0xde, 0xcc, 0xbe, 0x40, 0xdc,
    0xef, 0xbe, 0xaa, 0xf3, 0xed, 0xbe, 0xad, 0x9d, 0xe7, 0xf3, 0xad, 0xdc,
    0xef, 0x2d, 0xac, 0xf0, 0xee, 0xbf, 0x40, 0xdf, 0xef, 0xbe, 0xad, 0xde,
    0xfe, 0x93, 0x57, 0x00, 0xfe, 0x83, 0x17, 0x60, 0xe8, 0xa3, 0xa9, 0xde,
    0xed, 0xae, 0x99, 0x01, 0x62, 0x11, 0x00, 0x11, 0x11, 0x11, 0x22, 0x11,
    0x33, 0x24, 0x6d, 0xa6, 0x00, 0x24, 0x7d, 0xa6, 0x00, 0x2e, 0x38, 0x31
};

#endif // BOOTLOADER_H_INCLUDED
