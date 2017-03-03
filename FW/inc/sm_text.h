#ifndef SM_TEXT_H
#define SM_TEXT_H

#include <cstdint>

class SmText
{
public:
    uint16_t * pText;
    uint16_t length;

    SmText()
        :pText(nullptr),
        length(0)
    {}
} ;

#endif // SM_TEXT_H
