#ifndef SM_STRINGS_H
#define SM_STRINGS_H

#define FONT_IDX_SMALL  8
#define FONT_IDX_MEDIUM 3
#define FONT_IDX_DIGITS 4

// Do not include this file directly
// It should be parsed with flash_packer
// Special comment:
//     PACK is a flash_packer directive
//     SM_STRING_HELLO is a string definition
//     FONT_IDX_SMALL is a font index (should be defined in this file above)
//     The rest till the end of the line is just a string
// PACK SM_STRING_HELL FONT_IDX_SMALL HELL

#endif // SM_STRINGS_H
