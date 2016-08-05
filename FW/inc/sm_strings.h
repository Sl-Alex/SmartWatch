#ifndef SM_STRINGS_H
#define SM_STRINGS_H

#define FONT_IDX_SMALL  8
#define FONT_IDX_MEDIUM 3
#define FONT_IDX_DIGITS 4

// Special comment: PACK is a packer directive
// SM_STRING_HELLO is a string definition
// IDX_FW_FONT_SMALL
// PACK SM_STRING_HELL FONT_IDX_SMALL HELL
#define SM_STRING_HELL (uint16_t *)"\x28\x00\x25\x00\x2c\x00\x2c\x00"
#define SM_STRING_HELL_SZ 4


#endif // SM_STRINGS_H