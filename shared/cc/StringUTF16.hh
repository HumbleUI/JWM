#pragma once

#include <string>
#include <jni.h>

namespace jwm
{
    /**
     * @brief std::string which uses jchar as a character type
     */
    class StringUTF16: public std::basic_string<jchar> {
    public:
        using std::basic_string<jchar>::basic_string;

        static StringUTF16 makeFromUtf8(const char* str);
    };

} // namespace jwm
