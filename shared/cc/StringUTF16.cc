#include "StringUTF16.hh"
#include <vector>

jwm::StringUTF16::StringUTF16(const char* str) {
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
    for (uint32_t c : wide) {
        if (c <= 0xffff) {
            push_back(static_cast<jchar>(c));
        } else {
            c -= 0x10000;
            push_back(static_cast<jchar>((c >> 10) + 0xd800));
            push_back(static_cast<jchar>((c & 0x3ff) + 0xdc00));
        }
    }
}

jwm::StringUTF16::StringUTF16(const uint32_t *str) {
    for (uint32_t c = *str; c != 0; c = *(str += 1)) {
        if (c <= 0xffff) {
            push_back(static_cast<jchar>(c));
        } else {
            c -= 0x10000;
            push_back(static_cast<jchar>((c >> 10) + 0xd800));
            push_back(static_cast<jchar>((c & 0x3ff) + 0xdc00));
        }
    }
}

jwm::JNILocal<jstring> jwm::StringUTF16::toJString(JNIEnv* env) const {
    return jwm::JNILocal<jstring>(env, env->NewString(c_str(), static_cast<jsize>(length())));
}

jwm::StringUTF16 jwm::StringUTF16::makeFromJString(JNIEnv* env, jstring js) {
    jwm::StringUTF16 result;
    jsize length = env->GetStringLength(js);
    const jchar* chars = env->GetStringChars(js, nullptr);
    result = jwm::StringUTF16(chars, length);
    env->ReleaseStringChars(js, chars);
    return result;
}
std::string jwm::StringUTF16::toAscii() const {
    std::string result;
    result.resize(length());
    size_t i = 0;
    for (auto c : *this) {
        result[i++] = static_cast<char>(c);
    }
    return result;
}