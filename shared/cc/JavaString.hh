#pragma once

#include <string>
#include <jni.h>

namespace jwm
{
    /**
     * @brief std::string which uses jchar as a character type
     */
    class JavaString: public std::basic_string<jchar> {
    public:
        using std::basic_string<jchar>::basic_string;

        static JavaString makeFromUtf8(const char* str);
    };

} // namespace jwm
