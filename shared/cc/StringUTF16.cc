#include "StringUTF16.hh"
#include <vector>

jwm::StringUTF16 jwm::StringUTF16::makeFromUtf8(const char* str) {
    std::vector<uint32_t> wide;
    // convert from utf8 to 32-bit wide symbols
    while(*str)
    {
        if (*str & 0b10000000)
        {
            uint32_t t;
            // utf8 symbol
            if ((*str & 0b11110000) == 0b11110000)
            {
                // 4-byte symbol
                t = *(str++) & 0b111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                wide.push_back(t);
            } else if ((*str & 0b11100000) == 0b11100000)
            {
                // 3-byte symbol
                t = *(str++) & 0b1111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                wide.push_back(t);
            } else
            {
                // 2-byte symbol
                t = *(str++) & 0b11111;
                t <<= 6;
                t |= *(str++) & 0b111111;
                wide.push_back(t);
            }
        } else
        {
            // ascii symbol
            wide.push_back(*(str++));
        }
    }
    // convert from 32-bit wide symbols to utf16
    StringUTF16 utf16;
    for (uint32_t c : wide) {
        if (c <= 0xffff) {
            utf16 += static_cast<jchar>(c);
        } else {
            c -= 0x10000;
            utf16 += static_cast<jchar>((c >> 10) + 0xd800);
            utf16 += static_cast<jchar>((c & 0x3ff) + 0xdc00);
        }
    }

    return utf16;
}