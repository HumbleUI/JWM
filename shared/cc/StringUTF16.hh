#pragma once

#include <string>
#include <jni.h>
#include <impl/JNILocal.hh>
#include <cstdint>

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

        /**
         * Constructs StringUTF16 from C style string.
         * @arg str C style null terminated string in UTF32 encoding.
         */
        explicit StringUTF16(const uint32_t* str);

        StringUTF16() = default;

        static StringUTF16 makeFromJString(JNIEnv* env, jstring js);

        JNILocal<jstring> toJString(JNIEnv* env) const;

        std::string toAscii() const;

        bool operator==(const char* str) const {
            return (*this) == StringUTF16(str);
        }
    };

} // namespace jwm
