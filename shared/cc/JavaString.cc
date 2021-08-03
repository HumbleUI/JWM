#include "JavaString.hh"

jwm::JavaString jwm::JavaString::makeFromUtf8(const char* str) {
    jwm::JavaString destination;
    while(*str)
    {
        if (*str & 0x80)
        {
            jchar t;
            // utf8 symbol
            if (*str & 0b00100000)
            {
                // 3-byte symbol
                t = *(str++) & 0b1111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                destination.push_back(t);
            } else
            {
                // 2-byte symbol
                t = *(str++) & 0b11111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                destination.push_back(t);
            }
        } else
        {
            // ascii symbol
            destination.push_back(*(str++));
        }
    }
    return destination;
}