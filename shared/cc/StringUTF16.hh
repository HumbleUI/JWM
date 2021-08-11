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

        /**
         * Constructs StringUTF16 from C style string.
         * @arg str C style null terminated string in UTF8 encoding.
         */
        StringUTF16(const char* str);

        static StringUTF16 makeFromJString(JNIEnv* env, jstring js);

        bool operator==(const char* str) const {
            return (*this) == StringUTF16(str);
        }
    };

} // namespace jwm
